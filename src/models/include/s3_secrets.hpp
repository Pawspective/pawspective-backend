#pragma once

#include <string>
#include <userver/formats/json_fwd.hpp>
#include <userver/formats/parse/to.hpp>

namespace pawspective::models {

struct S3Secrets {
    explicit S3Secrets(const userver::formats::json::Value& value);
    std::string access_key;       // NOLINT [misc-non-private-member-variables-in-classes]
    std::string secret_key;       // NOLINT [misc-non-private-member-variables-in-classes]
    std::string bucket;           // NOLINT [misc-non-private-member-variables-in-classes]
    std::string endpoint;         // NOLINT [misc-non-private-member-variables-in-classes]
    std::string public_url_base;  // NOLINT [misc-non-private-member-variables-in-classes]
};

S3Secrets
Parse(const userver::formats::json::Value& value, userver::formats::parse::To<S3Secrets>);  // NOLINT
                                                                                            // [readability-identifier-naming]
}  // namespace pawspective::models
