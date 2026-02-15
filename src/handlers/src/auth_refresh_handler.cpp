#include "auth_refresh_handler.hpp"
#include <userver/formats/json/exception.hpp>
#include "pg_session_component.hpp"

namespace pawspective::handlers {

AuthRefreshHandler::AuthRefreshHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      session_component_(component_context.FindComponent<components::PgSessionComponent>()) {}

userver::formats::json::Value AuthRefreshHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& /*request*/,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& /*context*/
) const {
    std::string refresh_token;
    try {
        refresh_token = request_json["refresh_token"].As<std::string>();
    } catch (userver::formats::json::MemberMissingException& /*e*/) {
        throw userver::server::handlers::ClientError{};
    }

    auto payload = session_component_.get_service().validate_session(refresh_token);
    if (payload == std::nullopt) {
        LOG_WARNING() << "Invalid refresh token attempt";
        throw userver::server::handlers::Unauthorized(userver::server::handlers::ExternalBody{"Invalid refresh token"});
    }

    session_component_.get_service().revoke_session(refresh_token);
    services::SessionBundle new_tokens = session_component_.get_service().create_session(payload->user_id);

    LOG_INFO() << "Token rotated for user: " << payload->user_id;

    userver::formats::json::ValueBuilder response;
    response["access_token"] = new_tokens.access_token;
    response["refresh_token"] = new_tokens.refresh_token;
    response["token_type"] = "bearer";

    return response.ExtractValue();
}

}  // namespace pawspective::handlers