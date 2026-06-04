#include "upload_photo_handler.hpp"

#include <fmt/format.h>
#include <unordered_map>
#include <userver/formats/json/value_builder.hpp>
#include <userver/http/content_type.hpp>
#include <userver/logging/log.hpp>
#include <userver/utils/uuid4.hpp>
#include "s3_component.hpp"
#include "utils/error_response.hpp"

namespace pawspective::handlers {

namespace {

const std::unordered_map<std::string, std::string> kAllowedTypes{
    {"image/jpeg", "jpg"},
    {"image/png", "png"},
    {"image/webp", "webp"},
    {"image/gif", "gif"},
};

}  // namespace

UploadPhotoHandler::UploadPhotoHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context,
    bool is_monitor
)
    : HttpHandlerBase(config, context, is_monitor), s3_(context.FindComponent<components::S3Component>()) {}

std::string UploadPhotoHandler::HandleRequest(
    userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& /*context*/
) const {
    const auto media_type = std::string{userver::http::ContentType{request.GetHeader("Content-Type")}.MediaType()};

    const auto type_it = kAllowedTypes.find(media_type);
    if (type_it == kAllowedTypes.end()) {
        utils::ErrorResponse(
            utils::error_code::kInvalidContentType,
            "Unsupported content type. Allowed: image/jpeg, image/png, image/webp, image/gif"
        )
            .ThrowClientError();
    }

    const auto& body = request.RequestBody();
    if (body.empty()) {
        utils::ErrorResponse(utils::error_code::kMissingField, "Request body is empty").ThrowClientError();
    }

    const auto key = fmt::format("photos/{}.{}", userver::utils::generators::GenerateUuid(), type_it->second);

    std::string url;
    try {
        url = s3_.GetClient().UploadFile(key, body, media_type);
    } catch (const std::exception& e) {
        LOG_ERROR() << "Photo upload to S3 failed: " << e.what();
        utils::ErrorResponse(utils::error_code::kUploadFailed, "Failed to upload photo").ThrowClientError();
    }

    request.GetHttpResponse().SetContentType(userver::http::ContentType("application/json"));

    userver::formats::json::ValueBuilder builder;
    builder["url"] = url;
    return userver::formats::json::ToString(builder.ExtractValue());
}

}  // namespace pawspective::handlers
