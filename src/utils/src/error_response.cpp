#include "utils/error_response.hpp"

#include <userver/formats/json/value_builder.hpp>
#include <userver/logging/log.hpp>

namespace pawspective::utils {

ErrorResponse::ErrorResponse(const std::string& message) : message_(message) {}

userver::formats::json::Value ErrorResponse::GetJson(int http_status) const {
    userver::formats::json::ValueBuilder error;
    error["error"]["code"] = http_status;
    error["error"]["message"] = message_;
    return error.ExtractValue();
}

std::string ErrorResponse::GetString(int http_status) const {
    return userver::formats::json::ToString(GetJson(http_status));
}

void ErrorResponse::ThrowClientError() const {
    throw userver::server::handlers::ClientError{FormattedErrorBody{GetString(400)}};
}

void ErrorResponse::ThrowUnauthorized() const {
    throw userver::server::handlers::Unauthorized{FormattedErrorBody{GetString(401)}};
}

void ErrorResponse::ThrowConflict() const {
    throw userver::server::handlers::ConflictError{FormattedErrorBody{GetString(409)}};
}

void ErrorResponse::ThrowNotFound() const {
    throw userver::server::handlers::ResourceNotFound{FormattedErrorBody{GetString(404)}};
}

}  // namespace pawspective::utils
