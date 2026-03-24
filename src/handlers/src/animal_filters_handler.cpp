#include "animal_filters_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>

#include "animal_filter_dto.hpp"
#include "animal_service_component.hpp"

namespace pawspective::handlers {

AnimalFiltersHandler::AnimalFiltersHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      animal_service_(component_context.FindComponent<components::AnimalServiceComponent>()) {}

userver::formats::json::Value AnimalFiltersHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& /*request*/,
    const userver::formats::json::Value& /*request_json*/,
    userver::server::request::RequestContext& /*context*/
) const {
    const auto filters = animal_service_.get_service().GetFilterOptions();
    return userver::formats::json::ValueBuilder{filters}.ExtractValue();
}

}  // namespace pawspective::handlers