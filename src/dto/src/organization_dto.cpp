// cppcheck-suppress-file style
#include "../include/organization_dto.hpp"
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

[[maybe_unused]] userver::formats::json::Value
Serialize(const OrganizationDTO& data, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;

    builder["id"] = data.id;
    builder["name"] = data.name;
    builder["description"] = data.description;
    builder["city"] = data.city;

    return builder.ExtractValue();
}

[[maybe_unused]] OrganizationDTO
Parse(const userver::formats::json::Value& json, userver::formats::parse::To<OrganizationDTO>) {
    OrganizationDTO dto;

    dto.id = json["id"].As<std::int64_t>();
    dto.name = json["name"].As<std::string>();
    dto.description = json["description"].As<std::optional<std::string>>();
    dto.city = json["city"].As<City>();

    return dto;
}

}  // namespace pawspective::dto
