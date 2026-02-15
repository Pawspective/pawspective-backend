#include "utils/exception.hpp"

namespace pawspective::utils {
ValidationException::ValidationException(std::vector<FieldError> errors)
    : std::runtime_error("Validation failed"), errors_(std::move(errors)) {}

const std::vector<ValidationException::FieldError>& ValidationException::GetErrors() const { return errors_; }
}  // namespace pawspective::utils