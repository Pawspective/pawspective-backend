#include "city_list_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>  // NOLINT
#include "city_service_component.hpp"

namespace pawspective::handlers {

CityListHandler::CityListHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      city_service_(component_context.FindComponent<components::CityServiceComponent>()) {}

userver::formats::json::Value CityListHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& /*request*/,
    const userver::formats::json::Value& /*request_json*/,
    userver::server::request::RequestContext& /*context*/
) const {
    const auto cities = city_service_.get_service().GetAll();
    return userver::formats::json::ValueBuilder{cities}.ExtractValue();
}

}  // namespace pawspective::handlers
