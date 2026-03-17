#include "org_search_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>  // NOLINT
#include "organization_service_component.hpp"

namespace pawspective::handlers {

OrgSearchHandler::OrgSearchHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      org_service_(component_context.FindComponent<components::OrganizationServiceComponent>()) {}

userver::formats::json::Value OrgSearchHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& /*request_json*/,
    userver::server::request::RequestContext& /*context*/
) const {
    const auto name = request.GetArg("name");
    const auto orgs = org_service_.get_service().FindByNameContaining(name);
    return userver::formats::json::ValueBuilder{orgs}.ExtractValue();
}

}  // namespace pawspective::handlers
