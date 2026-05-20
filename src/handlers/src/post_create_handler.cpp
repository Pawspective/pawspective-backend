#include "post_create_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/exception.hpp>
#include <userver/server/http/http_status.hpp>
#include "post_create_dto.hpp"
#include "post_service_component.hpp"
#include "services/exception.hpp"
#include "utils/error_response.hpp"
#include "utils/exception.hpp"
#include "utils/validator.hpp"

namespace pawspective::handlers {

PostCreateHandler::PostCreateHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      post_service_(component_context.FindComponent<components::PostServiceComponent>()) {}

userver::formats::json::Value PostCreateHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context
) const {
    const auto& user_id = context.GetData<int64_t>("user_id");

    dto::PostCreateDTO post_data;
    dto::PostDTO post_dto;
    try {
        post_data = request_json.As<dto::PostCreateDTO>();

        utils::Validator validator;
        validator.Field("text", post_data.text).NotBlank().MaxLength(255);
        validator.ThrowIfInvalid();

        post_dto = post_service_.get_service().Create(user_id, post_data);
    } catch (const userver::formats::json::MemberMissingException& e) {
        LOG_WARNING() << "Missing required field in post creation data: " << e.what();
        utils::ErrorResponse(utils::error_code::kMissingField, "Missing required field").ThrowClientError();
    } catch (const userver::formats::json::Exception& e) {
        LOG_WARNING() << "Invalid JSON format: " << e.what();
        utils::ErrorResponse(utils::error_code::kInvalidJsonFormat, "Invalid JSON format").ThrowClientError();
    } catch (const utils::ValidationException& e) {
        LOG_WARNING() << "Validation failed for post creation.";
        e.ThrowClientError();
    } catch (const services::ForbiddenException& e) {
        LOG_WARNING() << "User " << user_id << " is not allowed to create post: " << e.what();
        utils::ErrorResponse(utils::error_code::kForbidden, "User is not allowed to create a post").ThrowForbidden();
    }

    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
    return userver::formats::json::ValueBuilder{post_dto}.ExtractValue();
}

}  // namespace pawspective::handlers
