#include "breed_list_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>  // NOLINT
#include "animal_enums.hpp"
#include "breed_service_component.hpp"
#include "utils/error_response.hpp"

namespace pawspective::handlers {

BreedListHandler::BreedListHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      breed_service_(component_context.FindComponent<components::BreedServiceComponent>()) {}

userver::formats::json::Value BreedListHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& /*request_json*/,
    userver::server::request::RequestContext& /*context*/
) const {
    const auto& type_str = request.GetArg("type");
    if (type_str.empty()) {
        utils::ErrorResponse(utils::error_code::kMissingField, "Query parameter 'type' is required").ThrowClientError();
    }

    models::AnimalType animal_type = models::AnimalType::kUnspecified;
    try {
        animal_type = userver::formats::json::ValueBuilder(type_str).ExtractValue().As<models::AnimalType>();
    } catch (const userver::formats::json::Exception&) {
        utils::ErrorResponse(utils::error_code::kInvalidJsonFormat, "Invalid animal type value").ThrowClientError();
    }

    const auto breeds = breed_service_.get_service().GetByType(animal_type);
    return userver::formats::json::ValueBuilder{breeds}.ExtractValue();
}

}  // namespace pawspective::handlers
