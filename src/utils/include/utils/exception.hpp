#pragma once

#include <stdexcept>
#include <string>
#include <userver/formats/json/value.hpp>
#include <vector>

namespace pawspective::utils {

class ValidationException : public std::runtime_error {
public:
    struct FieldError {
        std::string field_name;
        std::string error_message;
    };
    explicit ValidationException(std::vector<FieldError> errors);

    userver::formats::json::Value GetExternalResponse() const;

    const std::vector<FieldError>& GetErrors() const;

    /**
     * @brief Throw ClientError (HTTP 400) with the validation error response
     */
    [[noreturn]] void ThrowClientError() const;

private:
    const std::vector<FieldError> errors_;
};

}  // namespace pawspective::utils