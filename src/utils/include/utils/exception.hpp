#pragma once

#include <stdexcept>
#include <string>
#include <vector>

namespace pawspective::utils {

class ValidationException : public std::runtime_error {
public:
    struct FieldError {
        std::string field_name;
        std::string error_message;
    };
    explicit ValidationException(std::vector<FieldError> errors);

    const std::vector<FieldError>& GetErrors() const;

private:
    const std::vector<FieldError> errors_;
};

}  // namespace pawspective::utils