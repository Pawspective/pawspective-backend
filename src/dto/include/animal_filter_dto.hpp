#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include <userver/formats/json/value.hpp>
#include "../../models/include/animal_enums.hpp"

namespace pawspective::dto {

struct AnimalFilterDTO {
    std::optional<std::vector<std::int64_t>> breeds;
    std::optional<std::vector<std::int64_t>> city_ids;
    std::optional<std::vector<models::AnimalType>> animal_types;
    std::optional<std::vector<models::AnimalSize>> sizes;
    std::optional<std::vector<models::AnimalGender>> genders;
    std::optional<std::vector<models::CareLevel>> care_levels;
    std::optional<std::vector<models::AnimalColor>> colors;
    std::optional<std::vector<models::GoodWith>> good_withs;
    std::optional<std::vector<models::AnimalStatus>> statuses;
    std::optional<int> age_lte;
    std::optional<int> age_gte;
};

AnimalFilterDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<AnimalFilterDTO>);

userver::formats::json::Value
Serialize(const AnimalFilterDTO& filters, userver::formats::serialize::To<userver::formats::json::Value>);

}  // namespace pawspective::dto