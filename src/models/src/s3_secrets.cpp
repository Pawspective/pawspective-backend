#include "s3_secrets.hpp"
#include <userver/formats/json.hpp>  // NOLINT [misc-include-cleaner]

namespace pawspective::models {

S3Secrets
// NOLINTNEXTLINE [readability-identifier-naming]
Parse(const userver::formats::json::Value& value, userver::formats::parse::To<S3Secrets>) {
    return S3Secrets{value};
}

S3Secrets::S3Secrets(const userver::formats::json::Value& value) {
    if (!value.HasMember("s3-service")) {
        return;
    }
    const auto& s3 = value["s3-service"];
    access_key = s3["access_key"].As<std::string>("");
    secret_key = s3["secret_key"].As<std::string>("");
    bucket = s3["bucket"].As<std::string>("");
    endpoint = s3["endpoint"].As<std::string>("");
    public_url_base = s3["public_url_base"].As<std::string>("");
}

}  // namespace pawspective::models
