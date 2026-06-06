#include "animal_registration_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/exception.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/utils/regex.hpp>
#include "animal_register_dto.hpp"
#include "animal_service_component.hpp"
#include "services/exception.hpp"
#include "utils/error_response.hpp"
#include "utils/exception.hpp"
#include "utils/validator.hpp"

namespace pawspective::handlers {

AnimalRegistrationHandler::AnimalRegistrationHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      animal_service_(component_context.FindComponent<components::AnimalServiceComponent>()) {}

userver::formats::json::Value AnimalRegistrationHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context
) const {
    const auto& user_id = context.GetData<int64_t>("user_id");

    dto::AnimalRegisterDTO animal_data;
    dto::AnimalDTO animal_dto;
    try {
        animal_data = request_json.As<dto::AnimalRegisterDTO>();

        utils::Validator validator;
        validator.Field("name", animal_data.name).NotBlank().MaxLength(255);
        if (animal_data.description.has_value()) {
            validator.Field("description", *animal_data.description).MaxLength(2000);
        }
        validator.MaxSize(animal_data.photos.size(), 10, "must not exceed");
        for (size_t i = 0; i < animal_data.photos.size(); ++i) {
            validator.Field(fmt::format("photos[{}]", i), animal_data.photos[i])
                .MaxLength(2000)
                .Matches(
                    userver::utils::regex(R"(^[a-f0-9]{32}\.(jpg|jpeg|png|webp|gif)$)"),
                    "must be a valid photo filename (e.g. 66035e3bf.jpg)"
                );
        }
        validator.ThrowIfInvalid();

        animal_dto = animal_service_.get_service().Create(user_id, animal_data);
    } catch (const userver::formats::json::MemberMissingException& e) {
        LOG_WARNING() << "Missing required field in animal registration data: " << e.what();
        utils::ErrorResponse(utils::error_code::kMissingField, "Missing required field").ThrowClientError();
    } catch (const userver::formats::json::Exception& e) {
        LOG_WARNING() << "Invalid JSON format: " << e.what();
        utils::ErrorResponse(utils::error_code::kInvalidJsonFormat, "Invalid JSON format").ThrowClientError();
    } catch (const utils::ValidationException& e) {
        LOG_WARNING() << "Validation failed for animal registration.";
        e.ThrowClientError();
    } catch (const services::ForbiddenException& e) {
        LOG_WARNING() << "User " << user_id << " is not allowed to register animal: " << e.what();
        utils::ErrorResponse(utils::error_code::kForbidden, "User is not allowed to register an animal")
            .ThrowForbidden();
    } catch (const services::BreedNotFoundException&) {
        LOG_WARNING() << "Breed not found for breed_id: " << animal_data.breed_id;
        utils::ErrorResponse(utils::error_code::kBreedNotFound, "Breed not found").ThrowNotFound();
    }

    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
    return userver::formats::json::ValueBuilder{animal_dto}.ExtractValue();
}

}  // namespace pawspective::handlers
