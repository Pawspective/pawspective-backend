#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/parse/to.hpp>
#include <userver/formats/serialize/to.hpp>

namespace pawspective::dto {

struct UserDTO {
    std::int64_t id{};
    std::string email;
    std::string first_name;
    std::string last_name;
    std::optional<std::int64_t> organization_id;
};

userver::formats::json::Value
serialize(const UserDTO &data, userver::formats::serialize::To<userver::formats::json::Value>);
UserDTO
parse(const userver::formats::json::Value &json, userver::formats::parse::To<UserDTO>);

}  // namespace pawspective::dto
