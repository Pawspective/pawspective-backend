#include "s3_secrets.hpp"
#include <userver/formats/json.hpp>  // NOLINT [misc-include-cleaner]

namespace pawspective::models {

S3Secrets
// NOLINTNEXTLINE [readability-identifier-naming]
Parse(const userver::formats::json::Value& value, userver::formats::parse::To<S3Secrets>) {
    return S3Secrets{value};
}

S3Secrets::S3Secrets(const userver::formats::json::Value& value)
    : access_key(value["s3-service"]["access_key"].As<std::string>()),
      secret_key(value["s3-service"]["secret_key"].As<std::string>()) {}

}  // namespace pawspective::models
