#pragma once

#include <vector>

#include "city_service.hpp"
#include "organization_dto.hpp"
#include "organization_register_dto.hpp"
#include "organization_repository.hpp"
#include "organization_update_dto.hpp"

namespace pawspective::services {

class OrganizationService {
public:
    explicit OrganizationService(
        const repositories::OrganizationRepository& repo,
        const services::CityService& city_service
    );
    dto::OrganizationDTO Register(const dto::OrganizationRegisterDTO& dto) const;
    [[nodiscard]] dto::OrganizationDTO Get(int64_t id) const;
    dto::OrganizationDTO Update(int64_t id, const dto::OrganizationUpdateDTO& dto) const;
    [[nodiscard]] std::vector<dto::OrganizationDTO> FindByNameContaining(const std::string& name) const;

private:
    const repositories::OrganizationRepository& repository_;
    const services::CityService& city_service_;
};

}  // namespace pawspective::services
