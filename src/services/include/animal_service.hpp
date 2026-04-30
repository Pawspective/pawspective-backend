#pragma once

#include <cstdint>
#include <vector>

#include "animal_dto.hpp"
#include "animal_filter_dto.hpp"
#include "animal_list_dto.hpp"
#include "animal_register_dto.hpp"
#include "animal_repository.hpp"
#include "animal_update_dto.hpp"
#include "breed_service.hpp"
#include "organization_service.hpp"

namespace pawspective::services {

class AnimalService {
public:
    explicit AnimalService(
        const repositories::AnimalRepository& repo,
        const BreedService& breed_service,
        const OrganizationService& org_service,
        const UserService& user_service
    );

    dto::AnimalDTO Create(int64_t user_id, const dto::AnimalRegisterDTO& dto) const;
    dto::AnimalDTO Update(int64_t user_id, int64_t animal_id, const dto::AnimalUpdateDTO& dto) const;
    [[nodiscard]] dto::AnimalDTO Get(int64_t id) const;
    [[nodiscard]] std::vector<dto::AnimalDTO> GetByOrganization(int64_t org_id) const;
    [[nodiscard]] dto::AnimalFilterDTO GetFilterOptions() const;
    [[nodiscard]] dto::AnimalListDTO FindByFilters(const dto::AnimalFilterDTO& dto, int page) const;

private:
    const repositories::AnimalRepository& repository_;
    const BreedService& breed_service_;
    const OrganizationService& org_service_;
    const UserService& user_service_;
};

}  // namespace pawspective::services