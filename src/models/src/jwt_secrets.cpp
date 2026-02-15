#include "jwt_secrets.hpp"
#include <string>
#include <userver/formats/json.hpp>  // NOLINT [misc-include-cleaner]

namespace pawspective::models {
JwtSecrets
// NOLINTNEXTLINE [readability-identifier-naming]
Parse(const userver::formats::json::Value& value, userver::formats::parse::To<JwtSecrets>) {
    return JwtSecrets{value};
}

JwtSecrets::JwtSecrets(const userver::formats::json::Value& value)
    : secret_key(value["jwt-service"]["secret_key"].As<std::string>()) {}

}  // namespace pawspective::models