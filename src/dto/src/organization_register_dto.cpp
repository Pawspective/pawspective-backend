// cppcheck-suppress-file style
#include "../include/organization_register_dto.hpp"
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
Serialize(const OrganizationRegisterDTO& data, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;

    builder["name"] = data.name;
    if (data.description.has_value()) {
        builder["description"] = *data.description;
    }
    builder["city"] = data.city;

    return builder.ExtractValue();
}

[[maybe_unused]] OrganizationRegisterDTO
Parse(const userver::formats::json::Value& json, userver::formats::parse::To<OrganizationRegisterDTO>) {
    OrganizationRegisterDTO dto;

    dto.name = json["name"].As<std::string>();
    if (json.HasMember("description")) {
        dto.description = json["description"].As<std::optional<std::string>>();
    }
    dto.city = json["city"].As<City>();

    return dto;
}

}  // namespace pawspective::dto
