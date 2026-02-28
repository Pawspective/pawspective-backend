#include "../include/auth_login_handler.hpp"
#include <userver/formats/json/exception.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include "services/exception.hpp"
#include "utils/error_response.hpp"
#include "utils/exception.hpp"
#include "utils/validator.hpp"

namespace pawspective::handlers {

AuthLoginHandler::AuthLoginHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      session_component_(component_context.FindComponent<components::PgSessionComponent>()),
      user_service_component_(component_context.FindComponent<components::UserServiceComponent>()) {
    LOG_INFO() << "AuthLoginHandler initialized";
}

userver::formats::json::Value AuthLoginHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& /*context*/
) const {
    std::string email;
    std::string password;

    try {
        email = request_json["email"].As<std::string>("");
        password = request_json["password"].As<std::string>("");
        utils::Validator validator;
        validator.Field("email", email)
            .NotEmpty()
            .Matches(
                userver::utils::regex{R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"},
                "Invalid email format"
            );
        validator.Field("password", password).NotEmpty().MinLength(8);
        validator.ThrowIfInvalid();
    } catch (const userver::formats::json::ParseException& e) {
        LOG_WARNING() << "Failed to parse login data: " << e.what();
        utils::ErrorResponse(utils::error_code::kInvalidJsonFormat, "Invalid JSON format").ThrowClientError();
    } catch (const userver::formats::json::MemberMissingException& e) {
        LOG_WARNING() << "Missing required field in login data: " << e.what();
        utils::ErrorResponse(utils::error_code::kMissingField, "Missing required field").ThrowClientError();
    } catch (const utils::ValidationException& e) {
        LOG_WARNING() << "Validation failed for login data";
        throw userver::server::handlers::ClientError{
            userver::server::handlers::ExternalBody{userver::formats::json::ToString(e.GetExternalResponse())}
        };
    }

    models::User user;
    try {
        user = user_service_component_.get_service().AuthenticateUser(email, password);
    } catch (const services::InvalidCredentialsException& e) {
        LOG_WARNING() << "Failed login attempt for email: " << email;
        utils::ErrorResponse(utils::error_code::kInvalidCredentials, "Invalid email or password").ThrowUnauthorized();
    } catch (const services::UserNotFoundException& e) {
        LOG_WARNING() << "Failed login attempt for email: " << email;
        utils::ErrorResponse(utils::error_code::kInvalidCredentials, "Invalid email or password").ThrowUnauthorized();
    }

    auto session = session_component_.get_service().create_session(user.id);
    userver::formats::json::ValueBuilder response;
    response["access_token"] = session.access_token;
    response["refresh_token"] = session.refresh_token;
    response["token_type"] = "bearer";

    LOG_INFO() << "User logged in successfully: " << user.id;
    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
    return response.ExtractValue();
}

}  // namespace pawspective::handlers