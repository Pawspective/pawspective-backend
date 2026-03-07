#include "city.hpp"

namespace pawspective::models {
City City::FromDTO(const dto::CityDTO& city_dto) {
    City city;
    city.id = city_dto.id;
    city.name = city_dto.name;
    return city;
}

dto::CityDTO City::ToDTO(const City& city) {
    dto::CityDTO city_dto;
    city_dto.id = city.id;
    city_dto.name = city.name;
    return city_dto;
}
}  // namespace pawspective::models
