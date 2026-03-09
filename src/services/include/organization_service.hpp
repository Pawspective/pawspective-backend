#pragma once

#include <vector>

#include "organization.hpp"
#include "organization_register_dto.hpp"
#include "organization_repository.hpp"
#include "organization_update_dto.hpp"

namespace pawspective::services {

class OrganizationService {
public:
    explicit OrganizationService(const repositories::OrganizationRepository& repo);
    models::Organization Register(const dto::OrganizationRegisterDTO& dto) const;
    [[nodiscard]] models::Organization Get(int64_t id) const;
    models::Organization Update(int64_t id, const dto::OrganizationUpdateDTO& dto) const;
    [[nodiscard]] std::vector<models::Organization> FindByNameContaining(const std::string& name) const;

private:
    const repositories::OrganizationRepository& repository_;
};

}  // namespace pawspective::services
