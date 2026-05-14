#pragma once

#include <vector>

#include "city_service.hpp"
#include "organization_dto.hpp"
#include "organization_list_dto.hpp"
#include "organization_register_dto.hpp"
#include "organization_repository.hpp"
#include "organization_update_dto.hpp"
#include "services/user_service.hpp"

namespace pawspective::services {

class OrganizationService {
public:
    explicit OrganizationService(
        const repositories::OrganizationRepository& repo,
        const services::CityService& city_service,
        const services::UserService& user_service
    );
    dto::OrganizationDTO Register(int64_t user_id, const dto::OrganizationRegisterDTO& dto) const;
    [[nodiscard]] dto::OrganizationDTO Get(int64_t id) const;
    dto::OrganizationDTO Update(int64_t user_id, int64_t org_id, const dto::OrganizationUpdateDTO& dto) const;
    [[nodiscard]] dto::OrganizationListDTO FindByNameContainingPaginated(const std::string& name, int page) const;
    void Delete(int64_t user_id, int64_t org_id) const;

private:
    const repositories::OrganizationRepository& repository_;
    const services::CityService& city_service_;
    const services::UserService& user_service_;
};

}  // namespace pawspective::services
