#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "animal_enums.hpp"

namespace pawspective::dto {

struct AnimalUpdateDTO {
    std::optional<std::string> name;
    std::optional<std::int64_t> breed_id;
    std::optional<std::vector<std::string>> photos;
    std::optional<models::AnimalSize> size;
    std::optional<models::AnimalGender> gender;
    std::optional<models::CareLevel> care_level;
    std::optional<models::AnimalColor> color;
    std::optional<models::GoodWith> good_with;
    std::optional<std::int32_t> age{};
    std::optional<std::string> description;
    std::optional<models::AnimalStatus> status;
};

userver::formats::json::Value
Serialize(const AnimalUpdateDTO& data, userver::formats::serialize::To<userver::formats::json::Value>);
AnimalUpdateDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<AnimalUpdateDTO>);

}  // namespace pawspective::dto
