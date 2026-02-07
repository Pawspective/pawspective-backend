#pragma once

#include <optional>
#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/parse/to.hpp>
#include <userver/formats/serialize/to.hpp>

namespace pawspective::dto {

struct UserUpdateDTO {
    std::optional<std::string> email;
    std::optional<std::string> password;
    std::optional<std::string> first_name;
    std::optional<std::string> last_name;
};

userver::formats::json::Value
serialize(const UserUpdateDTO &data, userver::formats::serialize::To<userver::formats::json::Value>);
UserUpdateDTO
parse(const userver::formats::json::Value &json, userver::formats::parse::To<UserUpdateDTO>);
}  // namespace pawspective::dto
