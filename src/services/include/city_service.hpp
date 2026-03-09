#pragma once

#include <vector>

#include "city_dto.hpp"
#include "city_repository.hpp"

namespace pawspective::services {

class CityService {
public:
    explicit CityService(const repositories::CityRepository& repo);
    [[nodiscard]] dto::CityDTO Get(int64_t id) const;
    [[nodiscard]] std::vector<dto::CityDTO> GetAll() const;

private:
    const repositories::CityRepository& repository_;
};

}  // namespace pawspective::services
