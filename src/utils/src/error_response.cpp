#include "utils/error_response.hpp"

#include <userver/formats/json/value_builder.hpp>
#include <userver/logging/log.hpp>

namespace pawspective::utils {

ErrorResponse::ErrorResponse(const std::string_view& code, const std::string& message)
    : code_(code), message_(message) {}

userver::formats::json::Value ErrorResponse::GetJson() const {
    userver::formats::json::ValueBuilder error;
    error["error"]["code"] = code_;
    error["error"]["message"] = message_;
    return error.ExtractValue();
}

std::string ErrorResponse::GetString() const { return userver::formats::json::ToString(GetJson()); }

void ErrorResponse::ThrowClientError() const {
    throw userver::server::handlers::ClientError{FormattedErrorBody{GetString()}};
}

void ErrorResponse::ThrowUnauthorized() const {
    throw userver::server::handlers::Unauthorized{FormattedErrorBody{GetString()}};
}

void ErrorResponse::ThrowForbidden() const {
    throw userver::server::handlers::ClientError{
        FormattedErrorBody{GetString()},
        userver::server::handlers::HandlerErrorCode::kForbidden
    };
}

void ErrorResponse::ThrowConflict() const {
    throw userver::server::handlers::ConflictError{FormattedErrorBody{GetString()}};
}

void ErrorResponse::ThrowNotFound() const {
    throw userver::server::handlers::ResourceNotFound{FormattedErrorBody{GetString()}};
}

void ErrorResponse::ThrowServerError() const {
    throw userver::server::handlers::InternalServerError{FormattedErrorBody{GetString()}};
}

}  // namespace pawspective::utils
