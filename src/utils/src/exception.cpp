#include "utils/exception.hpp"
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>

namespace pawspective::utils {
ValidationException::ValidationException(std::vector<FieldError> errors)
    : std::runtime_error("Validation failed"), errors_(std::move(errors)) {}

const std::vector<ValidationException::FieldError>& ValidationException::GetErrors() const { return errors_; }

userver::formats::json::Value ValidationException::GetExternalResponse() const {
    userver::formats::json::ValueBuilder builder;
    builder["status"] = "error";
    builder["message"] = "Validation constraints violated";

    userver::formats::json::ValueBuilder details(userver::formats::common::Type::kArray);
    for (const auto& error : errors_) {
        userver::formats::json::ValueBuilder item;
        item["field"] = error.field_name;
        item["error"] = error.error_message;
        details.PushBack(std::move(item));
    }
    builder["details"] = details;
    return builder.ExtractValue();
}

}  // namespace pawspective::utils