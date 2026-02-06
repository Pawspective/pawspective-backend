#pragma once

#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/parse/to.hpp>
#include <userver/formats/serialize/to.hpp>

namespace pawspective::dto {

struct UserRegisterDTO {
    std::string email;
    std::string password;
    std::string first_name;
    std::string last_name;
};

userver::formats::json::Value
serialize(const UserRegisterDTO &data, userver::formats::serialize::To<userver::formats::json::Value>);
UserRegisterDTO
parse(const userver::formats::json::Value &json, userver::formats::parse::To<UserRegisterDTO>);
}  // namespace pawspective::dto
