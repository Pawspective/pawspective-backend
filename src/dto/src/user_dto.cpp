// cppcheck-suppress-file style
#include "../include/user_dto.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/parse/common_containers.hpp>  // NOLINT
#include <userver/formats/parse/to.hpp>
#include <userver/formats/serialize/common_containers.hpp>  // NOLINT
#include <userver/formats/serialize/to.hpp>

namespace pawspective::dto {

// cppcheck-suppress unusedFunction
[[maybe_unused]] userver::formats::json::Value
serialize(const UserDTO &data, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;

    builder["id"] = data.id;
    builder["email"] = data.email;
    builder["first_name"] = data.first_name;
    builder["last_name"] = data.last_name;
    builder["organization_id"] = data.organization_id;

    return builder.ExtractValue();
}

// cppcheck-suppress unusedFunction
[[maybe_unused]] UserDTO
parse(const userver::formats::json::Value &json, userver::formats::parse::To<UserDTO>) {
    UserDTO dto;

    dto.id = json["id"].As<std::int64_t>();
    dto.email = json["email"].As<std::string>();
    dto.first_name = json["first_name"].As<std::string>();
    dto.last_name = json["last_name"].As<std::string>();
    dto.organization_id =
        json["organization_id"].As<std::optional<std::int64_t>>();

    return dto;
}

}  // namespace pawspective::dto
