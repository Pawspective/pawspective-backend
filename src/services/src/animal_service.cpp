#include "animal_service.hpp"

#include <algorithm>
#include <iterator>
#include "services/exception.hpp"

namespace pawspective::services {

namespace {
bool CanBeAdopted(const models::Animal& animal, const std::unordered_set<int64_t>& user_adopted_animal_ids) {
    return animal.status == models::AnimalStatus::kAvailable && !user_adopted_animal_ids.contains(animal.id);
}
}  // namespace

AnimalService::AnimalService(
    const repositories::AnimalRepository& repo,
    const BreedService& breed_service,
    const OrganizationService& org_service,
    const UserService& user_service,
    const repositories::AdoptRequestRepository& adopt_request_repo
)
    : repository_(repo),
      breed_service_(breed_service),
      org_service_(org_service),
      user_service_(user_service),
      adopt_request_repo_(adopt_request_repo) {}

dto::AnimalDTO AnimalService::Create(int64_t user_id, const dto::AnimalRegisterDTO& dto) const {
    auto user_org_id = user_service_.GetOrganizationId(user_id);
    if (!user_org_id || *user_org_id != dto.organization_id) {
        throw ForbiddenException();
    }

    auto breed_dto = breed_service_.Get(dto.breed_id);
    models::Animal animal = repository_.Create(models::Animal::from_register_dto(dto));
    return models::Animal::to_dto(animal, breed_dto, animal.status == models::AnimalStatus::kAvailable);
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
    std::unordered_set<int64_t> user_adopted_animal_ids = adopt_request_repo_.GetAnimalIdsByUserId(user_id);
    return models::Animal::to_dto(*updated, breed_dto, CanBeAdopted(*updated, user_adopted_animal_ids));
}

dto::AnimalDTO AnimalService::Adopt(int64_t user_id, int64_t animal_id) const {
    auto existing = repository_.GetById(animal_id);
    if (!existing) {
        throw AnimalNotFoundException();
    }
    if (existing->status != models::AnimalStatus::kAvailable) {
        throw AnimalNotAvailableException();
    }
    auto adopted = repository_.Adopt(animal_id, user_id);
    if (!adopted) {
        throw AnimalNotFoundException();
    }
    auto breed_dto = breed_service_.Get(adopted->breed_id);
    std::unordered_set<int64_t> user_adopted_animal_ids = adopt_request_repo_.GetAnimalIdsByUserId(user_id);
    return models::Animal::to_dto(*adopted, breed_dto, CanBeAdopted(*adopted, user_adopted_animal_ids));
}

std::vector<dto::AnimalDTO> AnimalService::GetByIds(
    const std::vector<std::int64_t>& ids,
    std::optional<int64_t> user_id
) const {
    auto animals = repository_.GetByIds(ids);

    std::vector<std::int64_t> breed_ids;
    breed_ids.reserve(animals.size());
    std::transform(animals.begin(), animals.end(), std::back_inserter(breed_ids), [](const auto& a) {
        return a.breed_id;
    });
    auto breed_dtos = breed_service_.GetByIds(breed_ids);

    std::vector<dto::AnimalDTO> result;
    result.reserve(animals.size());
    const bool has_user = user_id.has_value();
    std::unordered_set<int64_t> user_adopted_animal_ids;
    if (has_user) {
        user_adopted_animal_ids = adopt_request_repo_.GetAnimalIdsByUserId(*user_id);
    }
    std::transform(
        std::make_move_iterator(animals.begin()),
        std::make_move_iterator(animals.end()),
        std::back_inserter(result),
        [&breed_dtos, &user_adopted_animal_ids, has_user](models::Animal&& animal) {
            const bool can_be_adopted = has_user && CanBeAdopted(animal, user_adopted_animal_ids);
            return models::Animal::to_dto(std::move(animal), breed_dtos[animal.breed_id], can_be_adopted);
        }
    );
    return result;
}

dto::AnimalDTO AnimalService::Get(int64_t id, std::optional<int64_t> user_id) const {
    auto animal = repository_.GetById(id);
    if (!animal) {
        throw AnimalNotFoundException();
    }

    auto breed_dto = breed_service_.Get(animal->breed_id);
    const bool has_user = user_id.has_value();
    std::unordered_set<int64_t> user_adopted_animal_ids;
    if (has_user) {
        user_adopted_animal_ids = adopt_request_repo_.GetAnimalIdsByUserId(*user_id);
    }
    const bool can_be_adopted = has_user && CanBeAdopted(*animal, user_adopted_animal_ids);
    return models::Animal::to_dto(*animal, breed_dto, can_be_adopted);
}

dto::AnimalListDTO AnimalService::GetByOrganizationPaginated(int64_t org_id, int page, std::optional<int64_t> user_id)
    const {
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

    const bool has_user = user_id.has_value();
    std::unordered_set<int64_t> user_adopted_animal_ids;
    if (has_user) {
        user_adopted_animal_ids = adopt_request_repo_.GetAnimalIdsByUserId(*user_id);
    }
    std::transform(
        std::make_move_iterator(animals.begin()),
        std::make_move_iterator(animals.end()),
        std::back_inserter(items),
        [&breed_dtos, &user_adopted_animal_ids, has_user](models::Animal&& animal) {
            const bool can_be_adopted = has_user && CanBeAdopted(animal, user_adopted_animal_ids);
            return models::Animal::to_dto(std::move(animal), breed_dtos[animal.breed_id], can_be_adopted);
        }
    );

    const std::int64_t total_pages = total_count == 0 ? 1 : (total_count + kPageSize - 1) / kPageSize;
    return {std::move(items), page, kPageSize, total_count, total_pages};
}

std::unordered_map<int64_t, std::string> AnimalService::GetNames(const std::vector<int64_t>& ids) const {
    return repository_.GetNamesByIds(ids);
}

std::vector<dto::AnimalDTO> AnimalService::GetAdoptedAnimalsWithoutReviewsByUserId(int64_t user_id) const {
    auto animals = repository_.GetAdoptedAnimalsWithoutReviewsByUserId(user_id);

    std::vector<int64_t> breed_ids;
    breed_ids.reserve(animals.size());
    std::transform(animals.begin(), animals.end(), std::back_inserter(breed_ids), [](const auto& animal) {
        return animal.breed_id;
    });
    auto breed_dtos = breed_service_.GetByIds(breed_ids);

    std::vector<dto::AnimalDTO> result;
    result.reserve(animals.size());
    std::transform(
        std::make_move_iterator(animals.begin()),
        std::make_move_iterator(animals.end()),
        std::back_inserter(result),
        [&breed_dtos](models::Animal&& animal) {
            return models::Animal::to_dto(std::move(animal), breed_dtos[animal.breed_id], false);
        }
    );

    return result;
}

dto::AnimalFilterDTO AnimalService::GetFilterOptions() const {
    auto filters = repository_.GetAvailableFilters();
    return models::AnimalFilters::to_dto(filters);
}

dto::AnimalListDTO AnimalService::FindByFilters(
    const dto::AnimalFilterDTO& filter_dto,
    int page,
    std::optional<int64_t> user_id
) const {
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
    const bool has_user = user_id.has_value();
    std::unordered_set<int64_t> user_adopted_animal_ids;
    if (has_user) {
        user_adopted_animal_ids = adopt_request_repo_.GetAnimalIdsByUserId(*user_id);
    }
    std::transform(
        std::make_move_iterator(animals.begin()),
        std::make_move_iterator(animals.end()),
        std::back_inserter(items),
        [&breed_dtos, &user_adopted_animal_ids, has_user](models::Animal&& animal) {
            const bool can_be_adopted = has_user && CanBeAdopted(animal, user_adopted_animal_ids);
            return models::Animal::to_dto(std::move(animal), breed_dtos[animal.breed_id], can_be_adopted);
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
    auto deleted = repository_.Delete(animal_id);
    if (!deleted) {
        throw AnimalNotFoundException();
    }
}

}  // namespace pawspective::services