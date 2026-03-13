#include "org_get_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/exception.hpp>
#include <userver/server/http/http_status.hpp>
#include "organization_service_component.hpp"
#include "services/exception.hpp"
#include "utils/error_response.hpp"
#include "utils/validator.hpp"

namespace pawspective::handlers {

OrganizationGetHandler::OrganizationGetHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      org_service_(component_context.FindComponent<components::OrganizationServiceComponent>().get_service()) {}

userver::formats::json::Value OrganizationGetHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& /*request_json*/,
    userver::server::request::RequestContext& /*context*/
) const {
    const auto& id_str = request.GetPathArg("id");

    int64_t org_id{};
    try {
        org_id = std::stoll(id_str);
    } catch (const std::exception& e) {
        LOG_WARNING() << "Invalid organization ID format in path: " << id_str;
        utils::ErrorResponse(utils::error_code::kOrganizationNotFound, "Invalid organization ID format")
            .ThrowNotFound();
    }

    try {
        auto org_dto = org_service_.Get(org_id);
        return userver::formats::json::ValueBuilder{org_dto}.ExtractValue();
    } catch (const services::OrganizationNotFoundException& e) {
        LOG_WARNING() << "Organization not found for id: " << org_id;
        utils::ErrorResponse(utils::error_code::kOrganizationNotFound, "Organization not found").ThrowNotFound();
    } catch (const services::CityNotFoundException& e) {
        LOG_WARNING() << "City not found for organization id: " << org_id;
        utils::ErrorResponse(utils::error_code::kCityNotFound, "City not found").ThrowNotFound();
    }
}

}  // namespace pawspective::handlers
