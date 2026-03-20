#include "org_animals_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/exception.hpp>
#include <userver/server/http/http_status.hpp>
#include "animal_register_dto.hpp"
#include "animal_service_component.hpp"
#include "services/exception.hpp"
#include "utils/error_response.hpp"
#include "utils/exception.hpp"
#include "utils/validator.hpp"

namespace pawspective::handlers {

OrganizationAnimalHandler::OrganizationAnimalHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      animal_service_(component_context.FindComponent<components::AnimalServiceComponent>()) {}

userver::formats::json::Value OrganizationAnimalHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& /*request_json*/,
    userver::server::request::RequestContext& /*context*/
) const {
    const auto& id_str = request.GetPathArg("id");

    int64_t org_id = 0;
    try {
        org_id = std::stoll(id_str);
    } catch (const std::exception& e) {
        LOG_WARNING() << "Invalid organization ID format in path: " << id_str;
        utils::ErrorResponse(utils::error_code::kOrganizationNotFound, "Invalid organization ID format")
            .ThrowNotFound();
    }

    try {
        const std::vector<dto::AnimalDTO> animals = animal_service_.get_service().GetByOrganization(org_id);

        return userver::formats::json::ValueBuilder{animals}.ExtractValue();
    } catch (const services::OrganizationNotFoundException& e) {
        LOG_WARNING() << "Organization not found for ID: " << org_id;
        utils::ErrorResponse(utils::error_code::kOrganizationNotFound, "Organization not found").ThrowNotFound();
    }
}

}  // namespace pawspective::handlers
