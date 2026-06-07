#include "delete_photo_handler.hpp"

#include <userver/logging/log.hpp>
#include "s3_component.hpp"
#include "utils/error_response.hpp"

namespace pawspective::handlers {

DeletePhotoHandler::DeletePhotoHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, context, is_monitor), s3_(context.FindComponent<components::S3Component>()) {}

userver::formats::json::Value DeletePhotoHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_body,
    userver::server::request::RequestContext& /*context*/
) const {
    if (!request_body.HasMember("url") || request_body["url"].IsNull()) {
        utils::ErrorResponse(utils::error_code::kMissingField, "Field 'url' is required").ThrowClientError();
    }

    const auto url = request_body["url"].As<std::string>();
    if (url.empty()) {
        utils::ErrorResponse(utils::error_code::kMissingField, "Field 'url' must not be empty").ThrowClientError();
    }

    try {
        s3_.GetClient().DeleteFile(url);
    } catch (const std::invalid_argument&) {
        utils::ErrorResponse(utils::error_code::kValidationError, "Invalid photo URL").ThrowClientError();
    } catch (const std::exception& e) {
        LOG_ERROR() << "Photo delete from S3 failed: " << e.what();
        utils::ErrorResponse(utils::error_code::kDeleteFailed, "Failed to delete photo").ThrowServerError();
    }

    request.SetResponseStatus(userver::server::http::HttpStatus::kNoContent);
    return {};
}

}  // namespace pawspective::handlers
