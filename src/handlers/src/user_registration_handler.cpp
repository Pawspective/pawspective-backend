#include "user_registration_handler.hpp"

#include <algorithm>
#include <cctype>
#include <userver/components/component_context.hpp>
#include <userver/formats/json/exception.hpp>
#include <userver/server/http/http_status.hpp>
#include "components/user_service_component.hpp"
#include "services/exception.hpp"
#include "user.hpp"
#include "user_register_dto.hpp"
#include "utils/error_response.hpp"
#include "utils/exception.hpp"
#include "utils/validator.hpp"

namespace pawspective::handlers {

UserRegistrationHandler::UserRegistrationHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      user_service_(component_context.FindComponent<components::UserServiceComponent>()) {}

userver::formats::json::Value UserRegistrationHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& /*context*/
) const {
    dto::UserRegisterDTO user_data;
    models::User user;
    try {
        user_data = request_json.As<dto::UserRegisterDTO>();

        std::transform(user_data.email.begin(), user_data.email.end(), user_data.email.begin(), [](unsigned char c) {
            return std::tolower(c);
        });

        utils::Validator validator;
        validator.Field("email", user_data.email)
            .NotBlank()
            .MaxLength(255)
            .Matches(
                userver::utils::regex{R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"},
                "Invalid email format"
            );
        validator.Field("password", user_data.password).NotBlank().MinLength(8).MaxLength(100);
        validator.Field("first_name", user_data.first_name).NotBlank().MaxLength(255);
        validator.Field("last_name", user_data.last_name).NotBlank().MaxLength(255);
        validator.ThrowIfInvalid();
        user = user_service_.get_service().RegisterUser(user_data);
    } catch (const userver::formats::json::MemberMissingException& e) {
        LOG_WARNING() << "Missing required field in login data: " << e.what();
        utils::ErrorResponse(utils::error_code::kMissingField, "Missing required field").ThrowClientError();
    } catch (const userver::formats::json::Exception& e) {
        LOG_WARNING() << "Invalid JSON format: " << e.what();
        utils::ErrorResponse(utils::error_code::kInvalidJsonFormat, "Invalid JSON format").ThrowClientError();
    } catch (const utils::ValidationException& e) {
        LOG_WARNING() << "Validation failed for user registration.";
        e.ThrowClientError();
    } catch (services::UserAlreadyExistsException& /*e*/) {
        LOG_WARNING() << "Attempt to register with an existing email " << user_data.email;
        utils::ErrorResponse(utils::error_code::kUserAlreadyExists, "User with this email already exists")
            .ThrowConflict();
    }
    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
    return userver::formats::json::ValueBuilder{models::User::to_dto(user)}.ExtractValue();
}

}  // namespace pawspective::handlers