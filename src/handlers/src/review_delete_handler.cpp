#include "review_delete_handler.hpp"

#include <userver/components/component_context.hpp>
#include "services/exception.hpp"
#include "utils/error_response.hpp"

namespace pawspective::handlers {

ReviewDeleteHandler::ReviewDeleteHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : HttpHandlerJsonBase(config, component_context),
      review_service_(component_context.FindComponent<components::ReviewServiceComponent>()) {}

userver::formats::json::Value ReviewDeleteHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& /*request_body*/,
    userver::server::request::RequestContext& context
) const {
    const auto& auth_user_id = context.GetData<int64_t>("user_id");
    std::int64_t target_review_id = 0;

    try {
        target_review_id = std::stoll(request.GetPathArg("id"));
    } catch (const std::exception& e) {
        LOG_WARNING() << "Invalid review ID format in path: " << request.GetPathArg("id");
        utils::ErrorResponse(utils::error_code::kReviewNotFound, "Invalid review ID format").ThrowClientError();
    }

    try {
        review_service_.get_service().Delete(auth_user_id, target_review_id);
        request.SetResponseStatus(userver::server::http::HttpStatus::kNoContent);
        return {};
    } catch (const services::ForbiddenException&) {
        utils::ErrorResponse(utils::error_code::kForbidden, "You don't have permission to delete this review")
            .ThrowForbidden();
    } catch (const services::ReviewNotFoundException&) {
        utils::ErrorResponse(utils::error_code::kReviewNotFound, "Review not found").ThrowNotFound();
    }
}

}  // namespace pawspective::handlers
