#include "organization_service.hpp"

#include "services/exception.hpp"

namespace pawspective::services {

OrganizationService::OrganizationService(
    const repositories::OrganizationRepository& repo,
    const services::CityService& city_service,
    const services::UserService& user_service
)
    : repository_(repo), city_service_(city_service), user_service_(user_service) {}

dto::OrganizationDTO OrganizationService::Register(const int64_t user_id, const dto::OrganizationRegisterDTO& dto)
    const {
    if (!user_service_.CanUserCreateOrganization(user_id)) {
        throw ForbiddenException();
    }
    auto city_dto = city_service_.Get(dto.city_id);
    models::Organization org = repository_.Create(models::Organization::from_register_dto(dto));
    user_service_.LinkOrganization(user_id, org.id);

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
    std::vector<int64_t> city_ids;
    std::transform(orgs.begin(), orgs.end(), std::back_inserter(city_ids), [](const auto& org) { return org.city_id; });
    auto city_dtos = city_service_.GetByIds(city_ids);
    std::transform(
        std::make_move_iterator(orgs.begin()),
        std::make_move_iterator(orgs.end()),
        std::back_inserter(dtos),
        [&city_dtos](models::Organization&& org) {
            return models::Organization::to_dto(std::move(org), city_dtos[org.city_id]);
        }
    );
    return dtos;
}

dto::OrganizationDTO OrganizationService::Update(
    const int64_t user_id,
    const int64_t org_id,
    const dto::OrganizationUpdateDTO& dto
) const {
    auto user = user_service_.GetUserById(user_id);
    if (!user.organization_id || user.organization_id != org_id) {
        throw ForbiddenException();
    }
    std::optional<dto::CityDTO> validated_city;
    if (dto.city_id.has_value()) {
        validated_city = city_service_.Get(*dto.city_id);
    }
    models::Organization org = models::Organization::from_update_dto(org_id, dto);
    auto result = repository_.Update(org);
    if (!result) {
        throw OrganizationNotFoundException();
    }
    auto city_dto = validated_city.has_value() ? std::move(*validated_city) : city_service_.Get(result->city_id);
    return models::Organization::to_dto(*result, city_dto);
}

}  // namespace pawspective::services
