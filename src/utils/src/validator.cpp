#include "utils/validator.hpp"
#include <string>
#include <userver/utils/regex.hpp>
#include "utils/exception.hpp"

namespace pawspective::utils {

Validator& Validator::Field(std::string name, std::string value) {
    current_field_ = std::move(name);
    current_value_ = std::move(value);
    return *this;
}

Validator& Validator::NotEmpty() {
    if (current_value_.empty()) {
        AddError("must not be empty");
    }
    return *this;
}

Validator& Validator::MinLength(std::size_t min) {
    if (current_value_.length() < min) {
        AddError("must be at least " + std::to_string(min) + " characters long");
    }
    return *this;
}

Validator& Validator::Matches(const userver::utils::regex& re, std::string msg) {
    if (!userver::utils::regex_match(current_value_, re)) {
        AddError(std::move(msg));
    }
    return *this;
}

Validator& Validator::IsOneOf(const std::vector<std::string>& allowed) {
    if (std::find(allowed.begin(), allowed.end(), current_value_) == allowed.end()) {
        std::ostringstream oss;
        oss << "must be one of: [";
        for (size_t i = 0; i < allowed.size(); ++i) {
            if (i > 0) {
                oss << ", ";
            }
            oss << allowed[i];
        }
        oss << "]";

        AddError("must be one of: " + oss.str() + " allowed values");
    }
    return *this;
}

void Validator::ThrowIfInvalid() {
    if (!errors_.empty()) {
        throw ValidationException(errors_);
    }
}

void Validator::AddError(std::string msg) { errors_.push_back({current_field_, std::move(msg)}); }

}  // namespace pawspective::utils