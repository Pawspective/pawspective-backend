// cppcheck-suppress-file style
#include "../include/organization_update_dto.hpp"
#include <optional>
#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/parse/common_containers.hpp>  // NOLINT
#include <userver/formats/parse/to.hpp>
#include <userver/formats/serialize/common_containers.hpp>  // NOLINT
#include <userver/formats/serialize/to.hpp>

namespace pawspective::dto {

[[maybe_unused]] userver::formats::json::Value
Serialize(const OrganizationUpdateDTO& data, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;

    if (data.name.has_value()) {
        builder["name"] = *data.name;
    }
    if (data.description.has_value()) {
        builder["description"] = *data.description;
    }
    if (data.avatar_url.has_value()) {
        builder["avatar_url"] = *data.avatar_url;
    }
    if (data.city_id.has_value()) {
        builder["city_id"] = *data.city_id;
    }

    return builder.ExtractValue();
}

[[maybe_unused]] OrganizationUpdateDTO
Parse(const userver::formats::json::Value& json, userver::formats::parse::To<OrganizationUpdateDTO>) {
    OrganizationUpdateDTO dto;

    if (json.HasMember("name")) {
        dto.name = json["name"].As<std::optional<std::string>>();
    }
    if (json.HasMember("description")) {
        dto.description = json["description"].As<std::optional<std::string>>();
    }
    if (json.HasMember("avatar_url")) {
        dto.avatar_url = json["avatar_url"].As<std::optional<std::string>>();
    }
    if (json.HasMember("city_id")) {
        dto.city_id = json["city_id"].As<std::optional<std::int64_t>>();
    }

    return dto;
}

}  // namespace pawspective::dto
