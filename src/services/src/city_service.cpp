#include "city_service.hpp"

#include <algorithm>
#include <iterator>

#include "services/exception.hpp"

namespace pawspective::services {

CityService::CityService(const repositories::CityRepository& repo) : repository_(repo) {}

dto::CityDTO CityService::Get(int64_t id) const {
    auto city = repository_.GetById(id);
    if (!city) {
        throw CityNotFoundException();
    }

    return models::City::ToDTO(*city);
}

std::vector<dto::CityDTO> CityService::GetAll() const {
    auto cities = repository_.GetAll();

    std::vector<dto::CityDTO> dtos;
    dtos.reserve(cities.size());
    std::transform(cities.begin(), cities.end(), std::back_inserter(dtos), models::City::ToDTO);

    return dtos;
}

}  // namespace pawspective::services
