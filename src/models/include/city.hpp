#pragma once

#include <cstdint>
#include <string>
#include "city_dto.hpp"

namespace pawspective::models {

struct City {
    static City FromDTO(const dto::CityDTO& city_dto);
    static dto::CityDTO ToDTO(const City& city);

    std::int64_t id{};
    std::string name;
};

}  // namespace pawspective::models
