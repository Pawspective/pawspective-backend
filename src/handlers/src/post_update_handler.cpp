#include "post_update_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/exception.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/utils/regex.hpp>
#include "post_service_component.hpp"
#include "post_update_dto.hpp"
#include "services/exception.hpp"
#include "utils/error_response.hpp"
#include "utils/exception.hpp"
#include "utils/validator.hpp"

namespace pawspective::handlers {

PostUpdateHandler::PostUpdateHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      post_service_(component_context.FindComponent<components::PostServiceComponent>()) {}

userver::formats::json::Value PostUpdateHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context
) const {
    const auto& user_id = context.GetData<int64_t>("user_id");
    const auto& id_str = request.GetPathArg("id");
    std::int64_t post_id = 0;

    try {
        post_id = std::stoll(id_str);
    } catch (const std::exception& e) {
        LOG_WARNING() << "Invalid post ID format in path: " << id_str;
        utils::ErrorResponse(utils::error_code::kPostNotFound, "Invalid post ID format").ThrowNotFound();
    }

    try {
        auto update_dto = request_json.As<dto::PostUpdateDTO>();

        utils::Validator validator;
        if (update_dto.text.has_value()) {
            validator.Field("text", *update_dto.text).NotBlank().MaxLength(2000);
        }
        if (update_dto.photos.has_value()) {
            const auto& photos_vector = *update_dto.photos;
            validator.MaxSize("photos", photos_vector.size(), 10, "must not exceed");
            static const auto kPhotoRegex = userver::utils::regex(R"(^.+\.(jpg|jpeg|png|webp|gif)$)");

            for (size_t i = 0; i < photos_vector.size(); ++i) {
                validator.Field(fmt::format("photos[{}]", i), photos_vector.at(i))
                    .MaxLength(2000)
                    .Matches(kPhotoRegex, "must be a valid photo filename with extension");
            }
        }
        validator.ThrowIfInvalid();

        auto post_dto = post_service_.get_service().Update(user_id, post_id, update_dto);
        return userver::formats::json::ValueBuilder{post_dto}.ExtractValue();

    } catch (const userver::formats::json::MemberMissingException& e) {
        LOG_WARNING() << "Missing required field in post update data: " << e.what();
        utils::ErrorResponse(utils::error_code::kMissingField, "Missing required field").ThrowClientError();
    } catch (const userver::formats::json::Exception& e) {
        LOG_WARNING() << "Invalid JSON format: " << e.what();
        utils::ErrorResponse(utils::error_code::kInvalidJsonFormat, "Invalid JSON format").ThrowClientError();
    } catch (const utils::ValidationException& e) {
        LOG_WARNING() << "Validation failed for post update.";
        e.ThrowClientError();
    } catch (const services::PostNotFoundException&) {
        LOG_WARNING() << "Post not found: " << post_id;
        utils::ErrorResponse(utils::error_code::kPostNotFound, "Post not found").ThrowNotFound();
    } catch (const services::ForbiddenException& e) {
        LOG_WARNING() << "User " << user_id << " is not allowed to update post " << post_id << ": " << e.what();
        utils::ErrorResponse(utils::error_code::kForbidden, "User is not allowed to update this post").ThrowForbidden();
    }
}

}  // namespace pawspective::handlers
