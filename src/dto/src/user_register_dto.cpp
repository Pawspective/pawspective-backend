// cppcheck-suppress all
#include "../include/user_register_dto.hpp"
#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/parse/to.hpp>
#include <userver/formats/serialize/to.hpp>

namespace pawspective::dto {

[[maybe_unused]] userver::formats::json::Value
serialize(const UserRegisterDTO &data, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;

    builder["email"] = data.email;
    builder["password"] = data.password;
    builder["first_name"] = data.first_name;
    builder["last_name"] = data.last_name;

    return builder.ExtractValue();
}

[[maybe_unused]] UserRegisterDTO
parse(const userver::formats::json::Value &json, userver::formats::parse::To<UserRegisterDTO>) {
    UserRegisterDTO dto;

    dto.email = json["email"].As<std::string>();
    dto.password = json["password"].As<std::string>();
    dto.first_name = json["first_name"].As<std::string>();
    dto.last_name = json["last_name"].As<std::string>();

    return dto;
}

}  // namespace pawspective::dto