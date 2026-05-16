#pragma once

#include <cstdint>
#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/parse/to.hpp>
#include <userver/formats/serialize/to.hpp>

#include "animal_dto.hpp"

namespace pawspective::dto {

struct AdoptRequestDTO {
    std::int64_t id{};
    std::string email;
    AnimalDTO animal;
};

userver::formats::json::Value
Serialize(const AdoptRequestDTO& data, userver::formats::serialize::To<userver::formats::json::Value>);
AdoptRequestDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<AdoptRequestDTO>);

}  // namespace pawspective::dto
