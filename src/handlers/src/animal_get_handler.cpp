#include "animal_get_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/exception.hpp>
#include <userver/server/http/http_status.hpp>
#include "animal_service_component.hpp"
#include "services/exception.hpp"
#include "utils/error_response.hpp"
#include "utils/validator.hpp"

namespace pawspective::handlers {

AnimalGetHandler::AnimalGetHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      animal_service_(component_context.FindComponent<components::AnimalServiceComponent>()) {}

userver::formats::json::Value AnimalGetHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& /*request_json*/,
    userver::server::request::RequestContext& /*context*/
) const {
    const auto& id_str = request.GetPathArg("id");

    int64_t animal_id = 0;
    try {
        animal_id = std::stoll(id_str);
    } catch (const std::exception& e) {
        LOG_WARNING() << "Invalid animal ID format in path: " << id_str;
        utils::ErrorResponse(utils::error_code::kAnimalNotFound, "Invalid animal ID format").ThrowNotFound();
    }

    try {
        const dto::AnimalDTO animal_dto = animal_service_.get_service().Get(animal_id);

        return userver::formats::json::ValueBuilder{animal_dto}.ExtractValue();
    } catch (const services::AnimalNotFoundException& e) {
        LOG_WARNING() << "Animal not found for ID: " << animal_id;
        utils::ErrorResponse(utils::error_code::kAnimalNotFound, "Animal not found").ThrowNotFound();
    } catch (const services::BreedNotFoundException& e) {
        LOG_WARNING() << "Breed not found for animal ID: " << animal_id;
        utils::ErrorResponse(utils::error_code::kBreedNotFound, "Breed not found").ThrowNotFound();
    }
}

}  // namespace pawspective::handlers
