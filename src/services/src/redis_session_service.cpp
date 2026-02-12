#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <userver/crypto/hash.hpp>
#include <userver/storages/redis/client.hpp>  // NOLINT(misc-include-cleaner)
#include <userver/storages/redis/client_fwd.hpp>
#include <utility>
#include "jwt_service.hpp"
#include "session_service.hpp"

namespace pawspective::services {

class RedisSessionService final : public SessionService {
public:
    RedisSessionService(
        userver::storages::redis::ClientPtr redis_client,
        JwtService jwt
    )
        : redis_(std::move(redis_client)), jwt_(std::move(jwt)) {
    }

    SessionBundle create_session(std::string_view user_id) override {
        const auto access_token = jwt_.generate_access_token(user_id);
        const auto refresh_token = jwt_.generate_refresh_token(user_id);

        SessionBundle bundle{access_token, refresh_token};

        auto hash = userver::crypto::hash::Sha256(bundle.refresh_token);

        redis_
            ->Setex(
                "sess:" + hash, std::chrono::hours(24 * 7),
                std::string(user_id), {}
            )
            .Get();

        return bundle;
    }

    std::optional<TokenPayload> validate_session(std::string_view refresh_token
    ) override {
        auto payload = jwt_.validate_refresh_token(refresh_token);
        if (!payload) {
            return std::nullopt;
        }

        auto hash = userver::crypto::hash::Sha256(refresh_token);
        auto exists = redis_->Get("sess:" + hash, {}).Get();

        return exists ? payload : std::nullopt;
    }

    void revoke_session(std::string_view refresh_token) override {
        auto hash = userver::crypto::hash::Sha256(refresh_token);
        redis_->Del("sess:" + hash, {}).Get();
    }

private:
    userver::storages::redis::ClientPtr redis_;
    JwtService jwt_;
};

}  // namespace pawspective::services