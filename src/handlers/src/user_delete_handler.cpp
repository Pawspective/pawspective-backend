#include "user_delete_handler.hpp"

#include <userver/components/component_context.hpp>
#include "services/exception.hpp"
#include "utils/error_response.hpp"
#include "utils/exception.hpp"

namespace pawspective::handlers {

UserDeleteHandler::UserDeleteHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : HttpHandlerJsonBase(config, component_context),
      user_service_(component_context.FindComponent<components::UserServiceComponent>()) {}

userver::formats::json::Value UserDeleteHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& /*request_body*/,
    userver::server::request::RequestContext& context
) const {
    const auto& auth_user_id = context.GetData<int64_t>("user_id");
    std::int64_t target_user_id = 0;
    try {
        target_user_id = std::stoll(request.GetPathArg("id"));
    } catch (const std::exception&) {
        utils::ErrorResponse(utils::error_code::kUserNotFound, "Invalid user id format").ThrowClientError();
    }

    if (auth_user_id != target_user_id) {
        utils::ErrorResponse(utils::error_code::kForbidden, "You can only delete your own account").ThrowForbidden();
    }

    try {
        user_service_.get_service().DeleteUser(target_user_id);
        request.SetResponseStatus(userver::server::http::HttpStatus::kNoContent);
        return {};
    } catch (const services::UserNotFoundException&) {
        utils::ErrorResponse(utils::error_code::kUserNotFound, "User not found").ThrowNotFound();
    }
}

}  // namespace pawspective::handlers