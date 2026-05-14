#include "animal_service.hpp"
#include "services/exception.hpp"

namespace pawspective::services {

AnimalService::AnimalService(
    const repositories::AnimalRepository& repo,
    const BreedService& breed_service,
    const OrganizationService& org_service,
    const UserService& user_service
)
    : repository_(repo), breed_service_(breed_service), org_service_(org_service), user_service_(user_service) {}

dto::AnimalDTO AnimalService::Create(int64_t user_id, const dto::AnimalRegisterDTO& dto) const {
    auto user_org_id = user_service_.GetOrganizationId(user_id);
    if (!user_org_id || *user_org_id != dto.organization_id) {
        throw ForbiddenException();
    }

    auto breed_dto = breed_service_.Get(dto.breed_id);
    models::Animal animal = repository_.Create(models::Animal::from_register_dto(dto));
    return models::Animal::to_dto(animal, breed_dto);
}

dto::AnimalDTO AnimalService::Update(int64_t user_id, int64_t animal_id, const dto::AnimalUpdateDTO& dto) const {
    auto existing = repository_.GetById(animal_id);
    if (!existing) {
        throw AnimalNotFoundException();
    }

    auto user_org_id = user_service_.GetOrganizationId(user_id);
    if (!user_org_id || *user_org_id != existing->organization_id) {
        throw ForbiddenException();
    }

    models::Animal to_update = models::Animal::from_update_dto(animal_id, dto);
    auto updated = repository_.Update(to_update);

    if (!updated) {
        throw AnimalNotFoundException();
    }

    auto breed_dto = breed_service_.Get(updated->breed_id);
    return models::Animal::to_dto(*updated, breed_dto);
}

dto::AnimalDTO AnimalService::Get(int64_t id) const {
    auto animal = repository_.GetById(id);
    if (!animal) {
        throw AnimalNotFoundException();
    }

    auto breed_dto = breed_service_.Get(animal->breed_id);
    return models::Animal::to_dto(*animal, breed_dto);
}

dto::AnimalListDTO AnimalService::GetByOrganizationPaginated(int64_t org_id, int page) const {
    static constexpr int kPageSize = 20;

    (void)org_service_.Get(org_id);

    auto [animals, total_count] = repository_.GetByOrganizationIdPaginated(org_id, page, kPageSize);

    std::vector<dto::AnimalDTO> items;
    items.reserve(animals.size());
    std::vector<int64_t> breed_ids;

    std::transform(animals.begin(), animals.end(), std::back_inserter(breed_ids), [](const auto& animal) {
        return animal.breed_id;
    });
    auto breed_dtos = breed_service_.GetByIds(breed_ids);
    std::transform(
        std::make_move_iterator(animals.begin()),
        std::make_move_iterator(animals.end()),
        std::back_inserter(items),
        [&breed_dtos](models::Animal&& animal) {
            return models::Animal::to_dto(std::move(animal), breed_dtos[animal.breed_id]);
        }
    );

    const std::int64_t total_pages = total_count == 0 ? 1 : (total_count + kPageSize - 1) / kPageSize;
    return {std::move(items), page, kPageSize, total_count, total_pages};
}

dto::AnimalFilterDTO AnimalService::GetFilterOptions() const {
    auto filters = repository_.GetAvailableFilters();
    return models::AnimalFilters::to_dto(filters);
}

dto::AnimalListDTO AnimalService::FindByFilters(const dto::AnimalFilterDTO& filter_dto, int page) const {
    static constexpr int kPageSize = 20;

    auto filter_model = models::AnimalFilters::from_dto(filter_dto);
    auto [animals, total_count] = repository_.FindByFiltersPaginated(filter_model, page, kPageSize);

    std::vector<dto::AnimalDTO> items;
    items.reserve(animals.size());
    std::vector<int64_t> breed_ids;

    std::transform(animals.begin(), animals.end(), std::back_inserter(breed_ids), [](const auto& animal) {
        return animal.breed_id;
    });
    auto breed_dtos = breed_service_.GetByIds(breed_ids);
    std::transform(
        std::make_move_iterator(animals.begin()),
        std::make_move_iterator(animals.end()),
        std::back_inserter(items),
        [&breed_dtos](models::Animal&& animal) {
            return models::Animal::to_dto(std::move(animal), breed_dtos[animal.breed_id]);
        }
    );

    const std::int64_t total_pages = total_count == 0 ? 1 : (total_count + kPageSize - 1) / kPageSize;
    return {std::move(items), page, kPageSize, total_count, total_pages};
}

void AnimalService::Delete(int64_t user_id, int64_t animal_id) const {
    auto existing = repository_.GetById(animal_id);
    if (!existing) {
        throw AnimalNotFoundException();
    }

    auto user_org_id = user_service_.GetOrganizationId(user_id);
    if (!user_org_id || *user_org_id != existing->organization_id) {
        throw ForbiddenException();
    }

    repository_.Delete(animal_id);
}

}  // namespace pawspective::services