#pragma once

#include <unordered_map>
#include <vector>
#include "breed_dto.hpp"
#include "breed_repository.hpp"

namespace pawspective::services {

class BreedService {
public:
    explicit BreedService(const repositories::BreedRepository& repo);

    [[nodiscard]] dto::BreedDTO Get(int64_t id) const;
    [[nodiscard]] std::vector<dto::BreedDTO> GetByType(models::AnimalType type) const;
    [[nodiscard]] std::unordered_map<int64_t, dto::BreedDTO> GetByIds(const std::vector<int64_t>& ids) const;

private:
    const repositories::BreedRepository& repository_;
};

}  // namespace pawspective::services