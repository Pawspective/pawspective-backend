#pragma once

#include <vector>

#include <userver/formats/json/value.hpp>
#include <userver/formats/serialize/to.hpp>

#include "animal_dto.hpp"

namespace pawspective::dto {

struct AnimalListDTO {
    std::vector<AnimalDTO> items;
    int page{};
    int limit{};
    int total_count{};
    int total_pages{};
};

userver::formats::json::Value
Serialize(const AnimalListDTO& data, userver::formats::serialize::To<userver::formats::json::Value>);

}  // namespace pawspective::dto
