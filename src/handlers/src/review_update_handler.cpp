#include "review_update_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/exception.hpp>
#include <userver/server/http/http_status.hpp>
#include "review_service_component.hpp"
#include "review_update_dto.hpp"
#include "services/exception.hpp"
#include "utils/error_response.hpp"
#include "utils/exception.hpp"
#include "utils/validator.hpp"

namespace pawspective::handlers {

ReviewUpdateHandler::ReviewUpdateHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      review_service_(component_context.FindComponent<components::ReviewServiceComponent>()) {}

userver::formats::json::Value ReviewUpdateHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context
) const {
    const auto& user_id = context.GetData<int64_t>("user_id");
    const auto& id_str = request.GetPathArg("id");
    std::int64_t review_id = 0;

    try {
        review_id = std::stoll(id_str);
    } catch (const std::exception&) {
        LOG_WARNING() << "Invalid review ID format in path: " << id_str;
        utils::ErrorResponse(utils::error_code::kReviewNotFound, "Invalid review ID format").ThrowNotFound();
    }

    try {
        auto update_dto = request_json.As<dto::ReviewUpdateDTO>();

        utils::Validator validator;
        if (update_dto.text.has_value()) {
            validator.Field("text", *update_dto.text).NotBlank().MaxLength(2000);
        }
        validator.ThrowIfInvalid();

        auto review_dto = review_service_.get_service().UpdateText(user_id, review_id, update_dto);
        return userver::formats::json::ValueBuilder{review_dto}.ExtractValue();

    } catch (const userver::formats::json::MemberMissingException& e) {
        LOG_WARNING() << "Missing required field in review update data: " << e.what();
        utils::ErrorResponse(utils::error_code::kMissingField, "Missing required field").ThrowClientError();
    } catch (const userver::formats::json::Exception& e) {
        LOG_WARNING() << "Invalid JSON format: " << e.what();
        utils::ErrorResponse(utils::error_code::kInvalidJsonFormat, "Invalid JSON format").ThrowClientError();
    } catch (const utils::ValidationException& e) {
        LOG_WARNING() << "Validation failed for review update.";
        e.ThrowClientError();
    } catch (const services::ReviewNotFoundException&) {
        LOG_WARNING() << "Review not found: " << review_id;
        utils::ErrorResponse(utils::error_code::kReviewNotFound, "Review not found").ThrowNotFound();
    } catch (const services::ForbiddenException& e) {
        LOG_WARNING() << "User " << user_id << " is not allowed to update review " << review_id << ": " << e.what();
        utils::ErrorResponse(utils::error_code::kForbidden, "User is not allowed to update this review")
            .ThrowForbidden();
    }
}

}  // namespace pawspective::handlers