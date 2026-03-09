#include "user_update_handler.hpp"

#include <algorithm>
#include <cctype>
#include <userver/components/component_context.hpp>
#include <userver/formats/json/exception.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/http/http_status.hpp>
#include "components/user_service_component.hpp"
#include "services/exception.hpp"
#include "utils/error_response.hpp"
#include "utils/exception.hpp"
#include "utils/validator.hpp"

namespace pawspective::handlers {

UserUpdateHandler::UserUpdateHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : HttpHandlerJsonBase(config, component_context),
      user_service_(component_context.FindComponent<components::UserServiceComponent>().get_service()) {}

userver::formats::json::Value UserUpdateHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_body,
    userver::server::request::RequestContext& context
) const {
    const auto& auth_user_id = context.GetData<int64_t>("user_id");

    const auto& id_str = request.GetPathArg("id");
    std::int64_t target_user_id = 0;

    try {
        target_user_id = std::stoll(id_str);
    } catch (const std::exception& e) {
        LOG_WARNING() << "Invalid user ID format in path: " << id_str;
        utils::ErrorResponse(utils::error_code::kUserNotFound, "Invalid user ID format").ThrowClientError();
    }

    if (auth_user_id != target_user_id) {
        LOG_WARNING()
            << "Unauthorized update attempt. User " << auth_user_id << " tried to update user " << target_user_id;
        utils::ErrorResponse(utils::error_code::kInvalidCredentials, "Cannot update other user's profile")
            .ThrowUnauthorized();
    }

    dto::UserUpdateDTO update_dto;
    try {
        update_dto = request_body.As<dto::UserUpdateDTO>();

        if (update_dto.email) {
            std::transform(
                update_dto.email->begin(),
                update_dto.email->end(),
                update_dto.email->begin(),
                [](unsigned char c) { return std::tolower(c); }
            );
        }

        utils::Validator validator;
        if (update_dto.email) {
            validator.Field("email", *update_dto.email)
                .NotBlank()
                .Matches(
                    userver::utils::regex{R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"},
                    "Invalid email format"
                );
        }
        if (update_dto.first_name) {
            validator.Field("first_name", *update_dto.first_name).NotBlank();
        }
        if (update_dto.last_name) {
            validator.Field("last_name", *update_dto.last_name).NotBlank();
        }
        validator.ThrowIfInvalid();

        LOG_INFO() << "Updating user " << target_user_id;
        auto updated_model = user_service_.UpdateUser(target_user_id, update_dto);

        auto response_dto = models::User::to_dto(updated_model);
        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);

        LOG_INFO() << "User " << target_user_id << " updated successfully";
        return userver::formats::json::ValueBuilder(response_dto).ExtractValue();

    } catch (const userver::formats::json::MemberMissingException& e) {
        LOG_WARNING() << "Missing required field in update data: " << e.what();
        utils::ErrorResponse(utils::error_code::kMissingField, "Missing required field").ThrowClientError();
    } catch (const userver::formats::json::Exception& e) {
        LOG_WARNING() << "Invalid JSON format: " << e.what();
        utils::ErrorResponse(utils::error_code::kInvalidJsonFormat, "Invalid JSON format").ThrowClientError();
    } catch (const utils::ValidationException& e) {
        LOG_WARNING() << "Validation failed for user update";
        e.ThrowClientError();
    } catch (const services::UserNotFoundException& e) {
        LOG_WARNING() << "User not found: " << target_user_id;
        utils::ErrorResponse(utils::error_code::kUserNotFound, "User not found").ThrowClientError();
    } catch (const services::UserAlreadyExistsException& e) {
        LOG_WARNING() << "Email already exists for another user";
        utils::ErrorResponse(utils::error_code::kUserAlreadyExists, "Email already in use").ThrowConflict();
    }
}
}  // namespace pawspective::handlers
