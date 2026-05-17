#include "adopt_request_service.hpp"

#include "adopt_request.hpp"
#include "services/exception.hpp"

namespace pawspective::services {

AdoptRequestService::AdoptRequestService(
    const repositories::AdoptRequestRepository& repo,
    const AnimalService& animal_service,
    const UserService& user_service
)
    : repository_(repo), animal_service_(animal_service), user_service_(user_service) {}

dto::AdoptRequestDTO AdoptRequestService::Create(std::int64_t user_id, std::int64_t animal_id) const {
    auto animal_dto = animal_service_.Get(animal_id);
    if (animal_dto.status != models::AnimalStatus::kAvailable) {
        throw AnimalNotAvailableException();
    }
    auto request = repository_.Create(animal_id, user_id);
    auto user = user_service_.GetUserById(user_id);
    return models::AdoptRequest::to_dto(request, user.email, animal_dto);
}

dto::AdoptRequestListDTO AdoptRequestService::GetByOrganizationId(std::int64_t user_id) const {
    auto org_id = user_service_.GetOrganizationId(user_id);
    if (!org_id) {
        throw ForbiddenException();
    }
    auto requests = repository_.GetByOrganizationId(*org_id);
    if (requests.empty()) {
        return {{}, 1, 0, 0, 1};
    }

    std::vector<std::int64_t> animal_ids;
    std::vector<std::int64_t> user_ids;
    animal_ids.reserve(requests.size());
    user_ids.reserve(requests.size());
    for (const auto& req : requests) {
        animal_ids.push_back(req.animal_id);
        user_ids.push_back(req.user_id);
    }

    auto animal_dtos = animal_service_.GetByIds(animal_ids);
    auto users = user_service_.GetUsersByIds(user_ids);

    std::unordered_map<std::int64_t, const dto::AnimalDTO*> animal_map;
    animal_map.reserve(animal_dtos.size());
    for (const auto& a : animal_dtos) {
        animal_map.emplace(a.id, &a);
    }

    std::unordered_map<std::int64_t, std::string> email_map;
    email_map.reserve(users.size());
    for (const auto& u : users) {
        email_map.emplace(u.id, u.email);
    }

    std::vector<dto::AdoptRequestDTO> items;
    items.reserve(requests.size());
    std::transform(requests.begin(), requests.end(), std::back_inserter(items), [&](const auto& req) {
        return models::AdoptRequest::to_dto(req, email_map.at(req.user_id), *animal_map.at(req.animal_id));
    });

    const auto total = static_cast<std::int64_t>(items.size());
    return {std::move(items), 1, static_cast<int>(total), total, 1};
}

void AdoptRequestService::Accept(std::int64_t org_user_id, std::int64_t request_id) const {
    auto request = repository_.GetById(request_id);
    if (!request) {
        throw AdoptRequestNotFoundException();
    }
    auto animal_dto = animal_service_.Get(request->animal_id);
    auto org_id_of_user = user_service_.GetOrganizationId(org_user_id);
    if (!org_id_of_user || *org_id_of_user != animal_dto.organization_id) {
        throw ForbiddenException();
    }
    animal_service_.Adopt(request->user_id, request->animal_id);
    repository_.DeleteByAnimalId(request->animal_id);
}

void AdoptRequestService::Deny(std::int64_t org_user_id, std::int64_t request_id) const {
    auto request = repository_.GetById(request_id);
    if (!request) {
        throw AdoptRequestNotFoundException();
    }
    auto animal_dto = animal_service_.Get(request->animal_id);
    auto org_id_of_user = user_service_.GetOrganizationId(org_user_id);
    if (!org_id_of_user || *org_id_of_user != animal_dto.organization_id) {
        throw ForbiddenException();
    }
    repository_.DeleteById(request_id);
}

}  // namespace pawspective::services
