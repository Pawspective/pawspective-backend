#pragma once

#include <optional>
#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/parse/to.hpp>
#include <userver/formats/serialize/to.hpp>
#include "city_dto.hpp"

namespace pawspective::dto {

struct OrganizationUpdateDTO {
    std::optional<std::string> name;
    std::optional<std::string> description;
    // std::optional<std::string> avatar_url; // TODO: add avatar_url later
    std::optional<City> city;
};

userver::formats::json::Value
Serialize(const OrganizationUpdateDTO& data, userver::formats::serialize::To<userver::formats::json::Value>);
OrganizationUpdateDTO
Parse(const userver::formats::json::Value& json, userver::formats::parse::To<OrganizationUpdateDTO>);

}  // namespace pawspective::dto
