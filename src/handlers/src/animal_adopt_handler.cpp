#include "animal_adopt_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/server/http/http_status.hpp>
#include "adopt_request_service_component.hpp"
#include "services/exception.hpp"
#include "utils/error_response.hpp"

namespace pawspective::handlers {

AnimalAdoptHandler::AnimalAdoptHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : HttpHandlerJsonBase(config, component_context),
      adopt_request_service_(component_context.FindComponent<components::AdoptRequestServiceComponent>()) {}

userver::formats::json::Value AnimalAdoptHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& /*request_json*/,
    userver::server::request::RequestContext& context
) const {
    const auto& user_id = context.GetData<int64_t>("user_id");

    std::int64_t animal_id = 0;
    try {
        animal_id = std::stoll(request.GetPathArg("id"));
    } catch (const std::exception&) {
        utils::ErrorResponse(utils::error_code::kValidationError, "Invalid animal ID format").ThrowClientError();
    }

    try {
        const auto result = adopt_request_service_.get_service().Create(user_id, animal_id);
        request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
        return userver::formats::json::ValueBuilder{result}.ExtractValue();
    } catch (const services::AnimalNotFoundException&) {
        utils::ErrorResponse(utils::error_code::kAnimalNotFound, "Animal not found").ThrowNotFound();
    } catch (const services::AnimalNotAvailableException&) {
        utils::ErrorResponse(utils::error_code::kAnimalNotAvailable, "Animal is not available for adoption")
            .ThrowConflict();
    } catch (const services::AdoptRequestAlreadyExistsException&) {
        utils::ErrorResponse(utils::error_code::kAdoptRequestAlreadyExists, "Adopt request already exists")
            .ThrowConflict();
    }
}

}  // namespace pawspective::handlers
