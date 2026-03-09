#include "org_registration_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/exception.hpp>
#include <userver/server/http/http_status.hpp>
#include "city_dto.hpp"
#include "organization_service_component.hpp"
#include "organization.hpp"
#include "organization_register_dto.hpp"
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
    userver::server::request::RequestContext& /*context*/
) const {
    models::Organization org;
    dto::OrganizationRegisterDTO org_data;
    try {
        org_data = request_json.As<dto::OrganizationRegisterDTO>();

        utils::Validator validator;
        validator.Field("name", org_data.name).NotBlank();
        validator.ThrowIfInvalid();

        org = org_service_.get_service().Register(org_data);
    } catch (const userver::formats::json::MemberMissingException& e) {
        LOG_WARNING() << "Missing required field in organization registration data: " << e.what();
        utils::ErrorResponse(utils::error_code::kMissingField, "Missing required field").ThrowClientError();
    } catch (const userver::formats::json::Exception& e) {
        LOG_WARNING() << "Invalid JSON format: " << e.what();
        utils::ErrorResponse(utils::error_code::kInvalidJsonFormat, "Invalid JSON format").ThrowClientError();
    } catch (const utils::ValidationException& e) {
        LOG_WARNING() << "Validation failed for organization registration.";
        e.ThrowClientError();
    }

    // Stub: city name is not fetched — only city_id is available from the created record.
    dto::CityDTO city_dto{org.city_id, ""};
    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
    return userver::formats::json::ValueBuilder{models::Organization::to_dto(org, city_dto)}.ExtractValue();
}

}  // namespace pawspective::handlers
