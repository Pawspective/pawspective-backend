#include "../include/auth_logout_handler.hpp"
#include <userver/formats/json/exception.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include "utils/error_response.hpp"

namespace pawspective::handlers {

AuthLogoutHandler::AuthLogoutHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      session_component_(component_context.FindComponent<components::PgSessionComponent>()) {
    LOG_INFO() << "AuthLogoutHandler initialized";
}

userver::formats::json::Value AuthLogoutHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& /*request*/,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context
) const {
    int64_t user_id = context.GetData<int64_t>("user_id");
    std::string refresh_token;

    try {
        refresh_token = request_json["refresh_token"].As<std::string>();
    } catch (const userver::formats::json::ParseException& e) {
        LOG_WARNING() << "Failed to parse login data: " << e.what();
        utils::ErrorResponse("Invalid JSON format").ThrowClientError();
    } catch (const userver::formats::json::MemberMissingException& e) {
        LOG_WARNING() << "Missing required field in login data: " << e.what();
        utils::ErrorResponse("Missing required field").ThrowClientError();
    }

    session_component_.get_service().revoke_session(refresh_token);

    LOG_INFO() << "User logged out: " << user_id;
    return userver::formats::json::ValueBuilder{}.ExtractValue();
}

}  // namespace pawspective::handlers