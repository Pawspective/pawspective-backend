#pragma once

#include <cstdint>
#include <optional>
#include <string_view>
#include <userver/storages/postgres/cluster.hpp>
#include "jwt_service.hpp"
#include "session_service.hpp"

namespace pawspective::services {

class PgSessionService final : public SessionService {
public:
    explicit PgSessionService(userver::storages::postgres::ClusterPtr pg_cluster, const JwtService& jwt);

    SessionBundle create_session(std::int64_t user_id) const override;
    std::optional<TokenPayload> validate_session(std::string_view refresh_token) const override;
    void revoke_session(std::string_view refresh_token) const override;

private:
    const userver::storages::postgres::ClusterPtr pg_cluster_;
    const JwtService& jwt_;
};
}  // namespace pawspective::services