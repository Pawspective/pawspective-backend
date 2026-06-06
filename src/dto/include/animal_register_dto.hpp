#pragma once

#include <cstdint>
#include <string>
#include "animal_enums.hpp"

namespace pawspective::dto {

struct AnimalRegisterDTO {
    std::int64_t organization_id{};
    std::string name;
    std::vector<std::string> photos;
    std::int64_t breed_id{};
    models::AnimalSize size = models::AnimalSize::kUnspecified;
    models::AnimalGender gender = models::AnimalGender::kUnspecified;
    models::CareLevel care_level = models::CareLevel::kUnspecified;
    models::AnimalColor color = models::AnimalColor::kUnspecified;
    models::GoodWith good_with = models::GoodWith::kUnspecified;
    std::int32_t age{};
    std::optional<std::string> description;
    models::AnimalStatus status = models::AnimalStatus::kUnspecified;
};

userver::formats::json::Value
Serialize(const AnimalRegisterDTO& data, userver::formats::serialize::To<userver::formats::json::Value>);
AnimalRegisterDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<AnimalRegisterDTO>);

}  // namespace pawspective::dto
