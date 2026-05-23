#include "adopt_request_accept_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/server/http/http_status.hpp>
#include "adopt_request_service_component.hpp"
#include "services/exception.hpp"
#include "utils/error_response.hpp"

namespace pawspective::handlers {

AdoptRequestAcceptHandler::AdoptRequestAcceptHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : HttpHandlerJsonBase(config, component_context),
      adopt_request_service_(component_context.FindComponent<components::AdoptRequestServiceComponent>()) {}

userver::formats::json::Value AdoptRequestAcceptHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& /*request_json*/,
    userver::server::request::RequestContext& context
) const {
    const auto& user_id = context.GetData<int64_t>("user_id");

    std::int64_t request_id = 0;
    try {
        request_id = std::stoll(request.GetPathArg("id"));
    } catch (const std::exception&) {
        utils::ErrorResponse(utils::error_code::kValidationError, "Invalid request ID format").ThrowClientError();
    }

    try {
        adopt_request_service_.get_service().Accept(user_id, request_id);
        request.SetResponseStatus(userver::server::http::HttpStatus::kNoContent);
        return {};
    } catch (const services::AdoptRequestNotFoundException&) {
        utils::ErrorResponse(utils::error_code::kForbidden, "Adopt request not found").ThrowNotFound();
    } catch (const services::ForbiddenException&) {
        utils::ErrorResponse(utils::error_code::kForbidden, "You do not have permission to accept this request")
            .ThrowForbidden();
    }
}

}  // namespace pawspective::handlers
