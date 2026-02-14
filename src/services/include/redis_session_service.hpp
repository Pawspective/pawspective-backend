#pragma once

#include <optional>
#include <string_view>
#include <userver/storages/redis/client.hpp>
#include "jwt_service.hpp"
#include "session_service.hpp"

namespace pawspective::services {

class RedisSessionService final : public SessionService {
    RedisSessionService(
        userver::storages::redis::ClientPtr redis_client,
        JwtService &jwt
    );

    SessionBundle create_session(std::string_view user_id) override;
    std::optional<TokenPayload> validate_session(std::string_view refresh_token
    ) override;
    void revoke_session(std::string_view refresh_token) override;

private:
    userver::storages::redis::ClientPtr redis_;
    JwtService &jwt_;
};

}  // namespace pawspective::services