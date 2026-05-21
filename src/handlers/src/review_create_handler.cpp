#include "review_create_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/exception.hpp>
#include <userver/server/http/http_status.hpp>
#include "review_create_dto.hpp"
#include "review_dto.hpp"
#include "services/exception.hpp"
#include "utils/error_response.hpp"
#include "utils/validator.hpp"

namespace pawspective::handlers {

ReviewCreateHandler::ReviewCreateHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      review_service_(component_context.FindComponent<components::ReviewServiceComponent>()) {}

userver::formats::json::Value ReviewCreateHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context
) const {
    const auto& user_id = context.GetData<int64_t>("user_id");

    dto::ReviewCreateDTO review_data;
    dto::ReviewDTO dto;
    try {
        review_data = request_json.As<dto::ReviewCreateDTO>();
        utils::Validator validator;
        validator.Field("text", review_data.text).NotBlank().MaxLength(2000);
        validator.ThrowIfInvalid();

        dto = review_service_.get_service().Create(user_id, review_data);
    } catch (const userver::formats::json::MemberMissingException& e) {
        LOG_WARNING() << "Missing required field in reveiw creation data: " << e.what();
        utils::ErrorResponse(utils::error_code::kMissingField, "Missing required field").ThrowClientError();
    } catch (const userver::formats::json::Exception& e) {
        LOG_WARNING() << "Invalid JSON format: " << e.what();
        utils::ErrorResponse(utils::error_code::kInvalidJsonFormat, "Invalid JSON format").ThrowClientError();
    } catch (const utils::ValidationException& e) {
        LOG_WARNING() << "Validation failed for review creation.";
        e.ThrowClientError();
    } catch (const services::ForbiddenException& e) {
        LOG_WARNING() << "User " << user_id << " is not allowed to create review: " << e.what();
        utils::ErrorResponse(utils::error_code::kForbidden, "User is not allowed to create a review").ThrowForbidden();
    } catch (const services::AnimalNotFoundException& e) {
        LOG_WARNING() << "Animal not found for review creation: " << e.what();
        utils::ErrorResponse(utils::error_code::kAnimalNotFound, "Animal not found").ThrowClientError();
    } catch (const services::OrganizationNotFoundException& e) {
        LOG_WARNING() << "Organization not found for review creation: " << e.what();
        utils::ErrorResponse(utils::error_code::kOrganizationNotFound, "Organization not found").ThrowClientError();
    } catch (const services::CityNotFoundException& e) {
        LOG_WARNING() << "City not found for review creation: " << e.what();
        utils::ErrorResponse(utils::error_code::kCityNotFound, "City not found").ThrowClientError();
    } catch (const services::BreedNotFoundException& e) {
        LOG_WARNING() << "Breed not found for review creation: " << e.what();
        utils::ErrorResponse(utils::error_code::kBreedNotFound, "Breed not found").ThrowClientError();
    } catch (const services::ReviewAlreadyExistsException& e) {
        LOG_WARNING() << "Review already exists for user " << user_id << ": " << e.what();
        utils::ErrorResponse(utils::error_code::kReviewAlreadyExists, "Review already exists").ThrowConflict();
    }
    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
    return userver::formats::json::ValueBuilder{dto}.ExtractValue();
}

}  // namespace pawspective::handlers
