#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <userver/formats/json/value.hpp>
#include <vector>

#include "breed_dto.hpp"

namespace pawspective::dto {

struct AnimalDTO {
    std::int64_t id{};
    std::int64_t organization_id{};
    std::string name;
    std::vector<std::string> photos;
    BreedDTO breed;
    models::AnimalSize size = models::AnimalSize::kUnspecified;
    models::AnimalGender gender = models::AnimalGender::kUnspecified;
    models::CareLevel care_level = models::CareLevel::kUnspecified;
    models::AnimalColor color = models::AnimalColor::kUnspecified;
    models::GoodWith good_with = models::GoodWith::kUnspecified;
    std::int32_t age{};
    std::optional<std::string> description;
    models::AnimalStatus status = models::AnimalStatus::kUnspecified;
    bool can_be_adopted = false;
};

userver::formats::json::Value
Serialize(const AnimalDTO& data, userver::formats::serialize::To<userver::formats::json::Value>);
AnimalDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<AnimalDTO>);

}  // namespace pawspective::dto
