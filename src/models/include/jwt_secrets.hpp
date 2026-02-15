#pragma once

#include <string>
#include <userver/formats/json_fwd.hpp>
#include <userver/formats/parse/to.hpp>

namespace pawspective::models {

struct JwtSecrets {
    explicit JwtSecrets(const userver::formats::json::Value& value);
    std::string secret_key;  // NOLINT [misc-non-private-member-variables-in-classes]
};

JwtSecrets Parse(const userver::formats::json::Value& value, userver::formats::parse::To<JwtSecrets>);  // NOLINT
                                                                                                        // [readability-identifier-naming]
}  // namespace pawspective::models