#include "posts_get_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/exception.hpp>
#include <userver/server/http/http_status.hpp>
#include "post_service_component.hpp"
#include "services/exception.hpp"
#include "utils/error_response.hpp"
#include "utils/exception.hpp"

namespace pawspective::handlers {

PostsGetHandler::PostsGetHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      post_service_(component_context.FindComponent<components::PostServiceComponent>()) {}

userver::formats::json::Value PostsGetHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& /*request_json*/,
    userver::server::request::RequestContext& /*context*/
) const {
    if (!request.HasPathArg("org_id")) {
        utils::ErrorResponse(utils::error_code::kMissingField, "org_id query parameter is required").ThrowClientError();
    }

    int64_t org_id = 0;
    try {
        org_id = std::stoll(std::string(request.GetArg("org_id")));
    } catch (const std::exception&) {
        utils::ErrorResponse(utils::error_code::kValidationError, "org_id must be a valid integer").ThrowClientError();
    }
    if (org_id < 1) {
        utils::ErrorResponse(utils::error_code::kValidationError, "org_id must be a positive integer")
            .ThrowClientError();
    }

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

    try {
        const auto result = post_service_.get_service().GetByOrganizationPaginated(org_id, page);
        return userver::formats::json::ValueBuilder{result}.ExtractValue();
    } catch (const services::OrganizationNotFoundException& e) {
        utils::ErrorResponse(utils::error_code::kOrganizationNotFound, "Organization not found").ThrowNotFound();
    }
}

}  // namespace pawspective::handlers
