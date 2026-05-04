#include "org_search_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>  // NOLINT
#include "organization_service_component.hpp"
#include "utils/error_response.hpp"

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
    const auto& name = request.GetArg("name");
    int page = 1;
    if (request.HasArg("page")) {
        try {
            page = std::stoi(std::string(request.GetArg("page")));
        } catch (const std::exception&) {
            utils::ErrorResponse(utils::error_code::kValidationError, "page must be a positive integer")
                .ThrowClientError();
        }
        if (page < 1) {
            utils::ErrorResponse(utils::error_code::kValidationError, "page must be a positive integer")
                .ThrowClientError();
        }
    }
    const auto result = org_service_.get_service().FindByNameContainingPaginated(name, page);
    return userver::formats::json::ValueBuilder{result}.ExtractValue();
}

}  // namespace pawspective::handlers
