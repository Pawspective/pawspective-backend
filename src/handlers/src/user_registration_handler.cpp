#include "user_registration_handler.hpp"

#include <userver/server/http/http_status.hpp>
#include <userver/formats/json/exception.hpp>
#include "services/exception.hpp"
#include "user_register_dto.hpp"
#include "utils/exception.hpp"

namespace pawspective::handlers {

// TODO: add in initialization of user service component
UserRegistrationHandler::UserRegistrationHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor) {}

userver::formats::json::Value UserRegistrationHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& /*context*/
) const {
    dto::UserRegisterDTO user_data;
    try {
        user_data = request_json.As<dto::UserRegisterDTO>();
        // TODO: add validation logic here or in a separate service layer
        request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
        return userver::formats::json::Value();

    } catch (userver::formats::json::ParseException& e) {
        LOG_WARNING() << "Failed to parse user registration data: " << e.what();
        throw userver::server::handlers::ClientError{userver::server::handlers::ExternalBody{"INVALID_JSON_FORMAT"}};
    } catch (userver::formats::json::MemberMissingException& e) {
        LOG_WARNING() << "Failed to parse user registration data: " << e.what();
        throw userver::server::handlers::ClientError{userver::server::handlers::ExternalBody{"INVALID_JSON_DATA"}};
    } catch (utils::ValidationException& e) {
        LOG_WARNING() << "Validation failed for user registration.";
        userver::formats::json::ValueBuilder error_response;
        error_response["code"] = "VALIDATION_ERROR";
        for (const auto& field_error : e.GetErrors()) {
            userver::formats::json::ValueBuilder error;
            error["field"] = field_error.field_name;
            error["message"] = field_error.error_message;
            error_response["errors"].PushBack(error.ExtractValue());
        }
        throw userver::server::handlers::ClientError{
            userver::server::handlers::ExternalBody{userver::formats::json::ToString(error_response.ExtractValue())}
        };
    } catch (services::UserAlreadyExistsException& /*e*/) {
        LOG_WARNING() << "Attempt to register with an existing email " << user_data.email;
        throw userver::server::handlers::ConflictError{userver::server::handlers::ExternalBody{"USER_ALREADY_EXISTS"}};
    }
}

}  // namespace pawspective::handlers