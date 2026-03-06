#pragma once

#include <optional>
#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/parse/to.hpp>
#include <userver/formats/serialize/to.hpp>
#include "city_dto.hpp"

namespace pawspective::dto {

struct OrganizationRegisterDTO {
    std::string name;
    std::optional<std::string> description;
    // std::optional<std::string> avatar_url; // TODO: add avatar_url later
    std::int64_t city_id{};
};

userver::formats::json::Value
Serialize(const OrganizationRegisterDTO& data, userver::formats::serialize::To<userver::formats::json::Value>);
OrganizationRegisterDTO
Parse(const userver::formats::json::Value& json, userver::formats::parse::To<OrganizationRegisterDTO>);

}  // namespace pawspective::dto
