#pragma once

#include <optional>
#include <string_view>
#include <userver/storages/postgres/cluster.hpp>
#include "jwt_service.hpp"
#include "session_service.hpp"

namespace pawspective::services {

class PgSessionService final : public SessionService {
    explicit PgSessionService(
        userver::storages::postgres::ClusterPtr pg_cluster,
        JwtService &jwt
    );

    SessionBundle create_session(std::string_view user_id) override;
    std::optional<TokenPayload> validate_session(std::string_view refresh_token
    ) override;
    void revoke_session(std::string_view refresh_token) override;

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
    JwtService &jwt_;
};
}  // namespace pawspective::services