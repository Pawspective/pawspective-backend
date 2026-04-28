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

std::unordered_map<int64_t, dto::CityDTO> CityService::GetByIds(const std::vector<int64_t>& ids) const {
    auto cities = repository_.GetByIds(ids);
    std::unordered_map<int64_t, dto::CityDTO> result;
    for (auto& city : cities) {
        result[city.id] = models::City::ToDTO(std::move(city));
    }
    return result;
}

}  // namespace pawspective::services
