#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/parse/to.hpp>
#include <userver/formats/serialize/to.hpp>
#include "city_dto.hpp"

namespace pawspective::dto {

struct OrganizationDTO {
    std::int64_t id{};
    std::string name;
    std::optional<std::string> description;
    // std::optional<std::string> avatar_url; // TODO: add avatar_url later
    City city;
};

userver::formats::json::Value
Serialize(const OrganizationDTO& data, userver::formats::serialize::To<userver::formats::json::Value>);
OrganizationDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<OrganizationDTO>);

}  // namespace pawspective::dto
