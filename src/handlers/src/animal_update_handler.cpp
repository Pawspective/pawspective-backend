#include "animal_update_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/exception.hpp>
#include <userver/server/http/http_status.hpp>
#include "animal_service_component.hpp"
#include "animal_update_dto.hpp"
#include "services/exception.hpp"
#include "utils/error_response.hpp"
#include "utils/exception.hpp"
#include "utils/validator.hpp"

namespace pawspective::handlers {

AnimalUpdateHandler::AnimalUpdateHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      animal_service_(component_context.FindComponent<components::AnimalServiceComponent>()) {}

userver::formats::json::Value AnimalUpdateHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context
) const {
    const auto& user_id = context.GetData<int64_t>("user_id");
    const auto& id_str = request.GetPathArg("id");
    std::int64_t animal_id = 0;

    try {
        animal_id = std::stoll(id_str);
    } catch (const std::exception& e) {
        LOG_WARNING() << "Invalid animal ID format in path: " << id_str;
        utils::ErrorResponse(utils::error_code::kAnimalNotFound, "Invalid animal ID format").ThrowNotFound();
    }

    try {
        auto update_dto = request_json.As<dto::AnimalUpdateDTO>();

        utils::Validator validator;
        if (update_dto.name) {
            validator.Field("name", *update_dto.name).NotBlank();
        }
        validator.ThrowIfInvalid();

        auto animal_dto = animal_service_.get_service().Update(user_id, animal_id, update_dto);
        return userver::formats::json::ValueBuilder{animal_dto}.ExtractValue();

    } catch (const userver::formats::json::MemberMissingException& e) {
        LOG_WARNING() << "Missing required field in animal update data: " << e.what();
        utils::ErrorResponse(utils::error_code::kMissingField, "Missing required field").ThrowClientError();
    } catch (const userver::formats::json::Exception& e) {
        LOG_WARNING() << "Invalid JSON format: " << e.what();
        utils::ErrorResponse(utils::error_code::kInvalidJsonFormat, "Invalid JSON format").ThrowClientError();
    } catch (const utils::ValidationException& e) {
        LOG_WARNING() << "Validation failed for animal update.";
        e.ThrowClientError();
    } catch (const services::AnimalNotFoundException&) {
        LOG_WARNING() << "Animal not found: " << animal_id;
        utils::ErrorResponse(utils::error_code::kAnimalNotFound, "Animal not found").ThrowNotFound();
    } catch (const services::ForbiddenException& e) {
        LOG_WARNING() << "User " << user_id << " is not allowed to update animal " << animal_id << ": " << e.what();
        utils::ErrorResponse(utils::error_code::kForbidden, "User is not allowed to update this animal")
            .ThrowForbidden();
    }
}

}  // namespace pawspective::handlers
