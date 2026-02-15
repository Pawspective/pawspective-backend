#include "../include/auth_logout_handler.hpp"
#include <userver/formats/json/exception.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/handlers/exceptions.hpp>

// #include "../../dto/include/user_dto.hpp"

namespace pawspective::handlers {

AuthLogoutHandler::AuthLogoutHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      session_component_(component_context.FindComponent<components::PgSessionComponent>()) {
    LOG_INFO() << "AuthLoginHandler initialized";
}

userver::formats::json::Value AuthLogoutHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context
) const {
    const auto& user_id = context.GetData<std::string>("user_id");
    std::string refresh_token;

    try {
        refresh_token = request_json["refresh_token"].As<std::string>();
    } catch (const userver::formats::json::MemberMissingException&) {
        LOG_WARNING() << "Missing refresh_token in request body for user: " << user_id;
        throw userver::server::handlers::Unauthorized(userver::server::handlers::ExternalBody{"Missing refresh_token"});
    }

    session_component_.get_service().revoke_session(refresh_token);

    LOG_INFO() << "User logged out: " << user_id;
    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
    return userver::formats::json::ValueBuilder{}.ExtractValue();
}

}  // namespace pawspective::handlers