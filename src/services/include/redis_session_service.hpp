#pragma once

#include <optional>
#include <string_view>
#include <userver/storages/redis/client.hpp>
#include "jwt_service.hpp"
#include "session_service.hpp"

namespace pawspective::services {

class RedisSessionService final : public SessionService {
public:
    RedisSessionService(userver::storages::redis::ClientPtr redis_client, const JwtService& jwt);

    SessionBundle create_session(std::string_view user_id) const override;
    std::optional<TokenPayload> validate_session(std::string_view refresh_token) const override;
    void revoke_session(std::string_view refresh_token) const override;

private:
    userver::storages::redis::ClientPtr redis_;
    const JwtService& jwt_;
};

}  // namespace pawspective::services