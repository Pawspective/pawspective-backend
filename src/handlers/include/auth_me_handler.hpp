#pragma once

#include <userver/components/component.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include "jwt_service.hpp"
#include "services/user_service.hpp"

namespace pawspective::handlers {

class AuthMeHandler final : public userver::server::handlers::HttpHandlerJsonBase {
public:
    constexpr static std::string_view kName = "handler-auth-me";
    AuthMeHandler(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context,
        bool is_monitor = false
    );
    userver::formats::json::Value HandleRequestJsonThrow(
        const userver::server::http::HttpRequest& request,
        const userver::formats::json::Value& request_json,
        userver::server::request::RequestContext& context
    ) const override;

private:
    const services::JwtService& jwt_service_;
    const services::UserService& user_service_;
};

}  // namespace pawspective::handlers
