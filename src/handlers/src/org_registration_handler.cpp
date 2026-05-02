#include "org_registration_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/exception.hpp>
#include <userver/server/http/http_status.hpp>
#include "organization_register_dto.hpp"
#include "organization_service_component.hpp"
#include "services/exception.hpp"
#include "utils/error_response.hpp"
#include "utils/exception.hpp"
#include "utils/validator.hpp"

namespace pawspective::handlers {

OrgRegistrationHandler::OrgRegistrationHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      org_service_(component_context.FindComponent<components::OrganizationServiceComponent>()) {}

userver::formats::json::Value OrgRegistrationHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context
) const {
    const auto& user_id = context.GetData<int64_t>("user_id");

    dto::OrganizationRegisterDTO org_data;
    dto::OrganizationDTO org_dto;
    try {
        org_data = request_json.As<dto::OrganizationRegisterDTO>();

        utils::Validator validator;
        validator.Field("name", org_data.name).NotBlank().MaxLength(255);
        if (org_data.description.has_value()) {
            validator.Field("description", *org_data.description).MaxLength(2000);
        }
        validator.ThrowIfInvalid();

        org_dto = org_service_.get_service().Register(user_id, org_data);
    } catch (const userver::formats::json::MemberMissingException& e) {
        LOG_WARNING() << "Missing required field in organization registration data: " << e.what();
        utils::ErrorResponse(utils::error_code::kMissingField, "Missing required field").ThrowClientError();
    } catch (const userver::formats::json::Exception& e) {
        LOG_WARNING() << "Invalid JSON format: " << e.what();
        utils::ErrorResponse(utils::error_code::kInvalidJsonFormat, "Invalid JSON format").ThrowClientError();
    } catch (const utils::ValidationException& e) {
        LOG_WARNING() << "Validation failed for organization registration.";
        e.ThrowClientError();
    } catch (const services::ForbiddenException& e) {
        LOG_WARNING() << "User " << user_id << " is not allowed to create organization: " << e.what();
        utils::ErrorResponse(utils::error_code::kForbidden, "User is not allowed to create an organization")
            .ThrowForbidden();
    } catch (const services::CityNotFoundException&) {
        LOG_WARNING() << "City not found for city_id: " << org_data.city_id;
        utils::ErrorResponse(utils::error_code::kCityNotFound, "City not found").ThrowNotFound();
    }

    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
    return userver::formats::json::ValueBuilder{org_dto}.ExtractValue();
}

}  // namespace pawspective::handlers
