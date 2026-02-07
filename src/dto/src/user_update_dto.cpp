// cppcheck-suppress all
#include "../include/user_update_dto.hpp"
#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/parse/to.hpp>
#include <userver/formats/serialize/to.hpp>

namespace pawspective::dto {

[[maybe_unused]] userver::formats::json::Value
serialize(const UserUpdateDTO &data, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;

    if (data.email.has_value()) {
        builder["email"] = *data.email;
    }
    if (data.password.has_value()) {
        builder["password"] = *data.password;
    }
    if (data.first_name.has_value()) {
        builder["first_name"] = *data.first_name;
    }
    if (data.last_name.has_value()) {
        builder["last_name"] = *data.last_name;
    }

    return builder.ExtractValue();
}

[[maybe_unused]] UserUpdateDTO
parse(const userver::formats::json::Value &json, userver::formats::parse::To<UserUpdateDTO>) {
    UserUpdateDTO dto;

    if (json.HasMember("email")) {
        dto.email = json["email"].As<std::string>();
    }
    if (json.HasMember("password")) {
        dto.password = json["password"].As<std::string>();
    }
    if (json.HasMember("first_name")) {
        dto.first_name = json["first_name"].As<std::string>();
    }
    if (json.HasMember("last_name")) {
        dto.last_name = json["last_name"].As<std::string>();
    }

    return dto;
}

}  // namespace pawspective::dto