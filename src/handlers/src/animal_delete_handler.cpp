#include "animal_delete_handler.hpp"

#include <userver/components/component_context.hpp>
#include "services/exception.hpp"
#include "utils/error_response.hpp"
#include "utils/exception.hpp"

namespace pawspective::handlers {

AnimalDeleteHandler::AnimalDeleteHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : HttpHandlerJsonBase(config, component_context),
      animal_service_(component_context.FindComponent<components::AnimalServiceComponent>()) {}

userver::formats::json::Value AnimalDeleteHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& /*request_body*/,
    userver::server::request::RequestContext& context
) const {
    const auto& auth_user_id = context.GetData<int64_t>("user_id");
    std::int64_t target_animal_id = 0;

    try {
        target_animal_id = std::stoll(request.GetPathArg("id"));
    } catch (const std::exception& e) {
        LOG_WARNING() << "Invalid animal ID format in path: " << request.GetPathArg("id");
        utils::ErrorResponse(utils::error_code::kAnimalNotFound, "Invalid animal ID format").ThrowClientError();
    }

    try {
        animal_service_.get_service().Delete(auth_user_id, target_animal_id);
        request.SetResponseStatus(userver::server::http::HttpStatus::kNoContent);
        return {};
    } catch (const services::ForbiddenException&) {
        utils::ErrorResponse(utils::error_code::kForbidden, "You don't have permission to delete this animal")
            .ThrowForbidden();
    } catch (const services::AnimalNotFoundException&) {
        utils::ErrorResponse(utils::error_code::kAnimalNotFound, "Animal not found").ThrowNotFound();
    }
}

}  // namespace pawspective::handlers