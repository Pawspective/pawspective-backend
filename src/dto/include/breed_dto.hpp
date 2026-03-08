#pragma once

#include <cstdint>
#include <string>

#include "animal_enums.hpp"

namespace pawspective::dto {
struct BreedDTO {
    std::int64_t id{};
    models::AnimalType animal_type = models::AnimalType::kOther;
    std::string name;
};

userver::formats::json::Value
Serialize(const BreedDTO& data, userver::formats::serialize::To<userver::formats::json::Value>);
BreedDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<BreedDTO>);

}  // namespace pawspective::dto
