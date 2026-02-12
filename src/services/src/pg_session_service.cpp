#include <optional>
#include <string_view>
#include <userver/crypto/hash.hpp>
#include <userver/storages/postgres/cluster.hpp>  // NOLINT(misc-include-cleaner)
#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/postgres_fwd.hpp>
#include <userver/storages/postgres/result_set.hpp>
#include <utility>
#include "jwt_service.hpp"
#include "session_service.hpp"

namespace pawspective::services {

class PgSessionService : public SessionService {
public:
    explicit PgSessionService(
        userver::storages::postgres::ClusterPtr pg_cluster,
        JwtService jwt
    )
        : pg_cluster_(std::move(pg_cluster)), jwt_(std::move(jwt)) {
    }

    SessionBundle create_session(std::string_view user_id) override {
        const auto access_token = jwt_.generate_access_token(user_id);
        const auto refresh_token = jwt_.generate_refresh_token(user_id);

        SessionBundle bundle{access_token, refresh_token};

        const auto hash = userver::crypto::hash::Sha256(bundle.refresh_token);

        pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            "INSERT INTO auth_schema.sessions (user_id, refresh_token_hash, "
            "expires_at) "
            "VALUES ($1, $2, NOW() + INTERVAL '7 days')",
            user_id, hash
        );

        return bundle;
    }

    std::optional<TokenPayload> validate_session(std::string_view refresh_token
    ) override {
        auto payload = jwt_.validate_refresh_token(refresh_token);

        if (!payload) {
            return std::nullopt;
        }

        const auto hash = userver::crypto::hash::Sha256(refresh_token);
        auto res = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kSlave,
            "SELECT 1 FROM auth_schema.sessions WHERE refresh_token_hash = $1 "
            "AND expires_at > NOW()",
            hash
        );

        return res.IsEmpty() ? std::nullopt : payload;
    }

    void revoke_session(std::string_view refresh_token) override {
        const auto hash = userver::crypto::hash::Sha256(refresh_token);
        pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            "DELETE FROM auth_schema.sessions WHERE refresh_token_hash = $1",
            hash
        );
    }

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
    JwtService jwt_;
};

}  // namespace pawspective::services