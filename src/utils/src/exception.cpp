#include "utils/exception.hpp"
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include "utils/error_response.hpp"

namespace pawspective::utils {
ValidationException::ValidationException(std::vector<FieldError> errors)
    : std::runtime_error("Validation failed"), errors_(std::move(errors)) {}

const std::vector<ValidationException::FieldError>& ValidationException::GetErrors() const { return errors_; }

void ValidationException::ThrowClientError() const {
    throw userver::server::handlers::ClientError{
        FormattedErrorBody{userver::formats::json::ToString(GetExternalResponse())}
    };
}

userver::formats::json::Value ValidationException::GetExternalResponse() const {
    userver::formats::json::ValueBuilder builder;
    builder["error"]["code"] = error_code::kValidationError;
    builder["error"]["message"] = "Validation failed";

    userver::formats::json::ValueBuilder details(userver::formats::common::Type::kObject);
    for (const auto& error : errors_) {
        details[error.field_name] = error.error_message;
    }
    builder["error"]["details"] = details;
    return builder.ExtractValue();
}

}  // namespace pawspective::utils