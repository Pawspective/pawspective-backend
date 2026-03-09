#include "organization_service.hpp"

#include "services/exception.hpp"

namespace pawspective::services {

OrganizationService::OrganizationService(
    const repositories::OrganizationRepository& repo,
    const services::CityService& city_service
)
    : repository_(repo), city_service_(city_service) {}

dto::OrganizationDTO OrganizationService::Register(const dto::OrganizationRegisterDTO& dto) const {
    auto city_dto = city_service_.Get(dto.city_id);
    models::Organization org = repository_.Create(models::Organization::from_register_dto(dto));

    return models::Organization::to_dto(org, city_dto);
}

dto::OrganizationDTO OrganizationService::Get(int64_t id) const {
    auto org = repository_.GetById(id);
    if (!org) {
        throw OrganizationNotFoundException();
    }
    auto city_dto = city_service_.Get(org->city_id);
    return models::Organization::to_dto(*org, city_dto);
}

std::vector<dto::OrganizationDTO> OrganizationService::FindByNameContaining(const std::string& name) const {
    auto orgs = repository_.FindByNameContaining(name);
    std::vector<dto::OrganizationDTO> dtos;
    for (const auto& org : orgs) {
        auto city_dto = city_service_.Get(org.city_id);
        dtos.push_back(models::Organization::to_dto(org, city_dto));
    }
    return dtos;
}

dto::OrganizationDTO OrganizationService::Update(int64_t id, const dto::OrganizationUpdateDTO& dto) const {
    models::Organization org = models::Organization::from_update_dto(id, dto);
    auto result = repository_.Update(org);
    if (!result) {
        throw OrganizationNotFoundException();
    }
    auto city_dto = city_service_.Get(result->city_id);
    return models::Organization::to_dto(*result, city_dto);
}

}  // namespace pawspective::services
