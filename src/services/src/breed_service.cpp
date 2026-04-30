#include "breed_service.hpp"

#include <algorithm>
#include <iterator>

#include "services/exception.hpp"

namespace pawspective::services {

BreedService::BreedService(const repositories::BreedRepository& repo) : repository_(repo) {}

dto::BreedDTO BreedService::Get(int64_t id) const {
    auto breed = repository_.GetById(id);
    if (!breed) {
        throw BreedNotFoundException();
    }
    return models::Breed::ToDTO(*breed);
}

std::vector<dto::BreedDTO> BreedService::GetByType(models::AnimalType type) const {
    auto breeds = repository_.GetByType(type);

    std::vector<dto::BreedDTO> dtos;
    dtos.reserve(breeds.size());
    std::transform(breeds.begin(), breeds.end(), std::back_inserter(dtos), models::Breed::ToDTO);

    return dtos;
}

std::unordered_map<int64_t, dto::BreedDTO> BreedService::GetByIds(const std::vector<int64_t>& ids) const {
    auto breeds = repository_.GetByIds(ids);
    std::unordered_map<int64_t, dto::BreedDTO> result;
    result.reserve(breeds.size());
    for (auto& breed : breeds) {
        result.emplace(breed.id, models::Breed::ToDTO(std::move(breed)));
    }
    return result;
}

}  // namespace pawspective::services