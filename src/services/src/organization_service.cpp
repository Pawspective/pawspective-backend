#include "organization_service.hpp"

#include "services/exception.hpp"

namespace pawspective::services {

OrganizationService::OrganizationService(const repositories::OrganizationRepository& repo) : repository_(repo) {}

models::Organization OrganizationService::Register(const dto::OrganizationRegisterDTO& dto) const {
    models::Organization org = models::Organization::from_register_dto(dto);
    return repository_.Create(org);
}

models::Organization OrganizationService::Get(int64_t id) const {
    auto org = repository_.GetById(id);
    if (!org) {
        throw OrganizationNotFoundException();
    }
    return *org;
}

std::vector<models::Organization> OrganizationService::FindByNameContaining(const std::string& name) const {
    return repository_.FindByNameContaining(name);
}

models::Organization OrganizationService::Update(int64_t id, const dto::OrganizationUpdateDTO& dto) const {
    models::Organization org = models::Organization::from_update_dto(id, dto);
    auto result = repository_.Update(org);
    if (!result) {
        throw OrganizationNotFoundException();
    }
    return *result;
}

}  // namespace pawspective::services
