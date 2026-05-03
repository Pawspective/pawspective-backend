#include "utils/validator.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <userver/utils/regex.hpp>
#include "utils/exception.hpp"

namespace pawspective::utils {

Validator& Validator::Field(std::string name, std::string value) {
    current_field_ = std::move(name);
    current_value_ = std::move(value);
    return *this;
}

Validator& Validator::NotBlank() {
    if (current_value_.empty() ||
        std::all_of(current_value_.begin(), current_value_.end(), [](unsigned char c) { return std::isspace(c); }))
    {
        AddError("must not be empty or whitespace-only");
    }
    return *this;
}

Validator& Validator::MinLength(std::size_t min) {
    if (current_value_.length() < min) {
        AddError("must be at least " + std::to_string(min) + " characters long");
    }
    return *this;
}

Validator& Validator::MaxLength(std::size_t max) {
    if (current_value_.length() > max) {
        AddError("must not exceed " + std::to_string(max) + " characters");
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