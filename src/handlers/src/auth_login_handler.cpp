#include "../include/auth_login_handler.hpp"
#include <userver/formats/json/exception.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/handlers/exceptions.hpp>

// #include "../../dto/include/user_dto.hpp"

namespace pawspective::handlers {

AuthLoginHandler::AuthLoginHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      session_component_(component_context.FindComponent<components::PgSessionComponent>(
      ) /*, user_service_(component_context.FindComponent<services::UserService>())*/) {
    LOG_INFO() << "AuthLoginHandler initialized";
}

userver::formats::json::Value AuthLoginHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& /*context*/
) const {
    std::string login;
    std::string password;

    try {
        login = request_json["login"].As<std::string>();
        password = request_json["password"].As<std::string>();
    } catch (const userver::formats::json::MemberMissingException&) {
        LOG_WARNING() << "Missing login or password in request";
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{"Missing login or password"
        });
    }

    // TODO: change when UserServise will be done(verify credentials)
    if (login != "demo" || password != "demo") {
        LOG_WARNING() << "Failed login attempt for login: " << login;
        throw userver::server::handlers::Unauthorized(userver::server::handlers::ExternalBody{
            "Invalid login or password"
        });
    }
    const std::string user_id = "123e4567-e89b-12d3-a456-426614174000";  // delete when UserServise will be done
    // const std::string user_id = std::to_string(user->id); // uncomment when UserServise will be done

    auto session = session_component_.get_service().create_session(user_id);
    userver::formats::json::ValueBuilder response;
    response["access_token"] = session.access_token;
    response["refresh_token"] = session.refresh_token;
    response["token_type"] = "bearer";
    LOG_INFO() << "User logged in successfully with stub: " << user_id;
    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
    return response.ExtractValue();
}

}  // namespace pawspective::handlers