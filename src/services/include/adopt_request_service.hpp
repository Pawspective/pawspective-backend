#pragma once

#include <cstdint>

#include "adopt_request_dto.hpp"
#include "adopt_request_list_dto.hpp"
#include "adopt_request_repository.hpp"
#include "animal_service.hpp"
#include "services/user_service.hpp"

namespace pawspective::services {

class AdoptRequestService {
public:
    explicit AdoptRequestService(
        const repositories::AdoptRequestRepository& repo,
        const AnimalService& animal_service,
        const UserService& user_service
    );

    dto::AdoptRequestDTO Create(std::int64_t user_id, std::int64_t animal_id) const;
    [[nodiscard]] dto::AdoptRequestListDTO GetByOrganizationId(std::int64_t org_user_id, std::int64_t org_id) const;
    void Accept(std::int64_t org_user_id, std::int64_t request_id) const;
    void Deny(std::int64_t org_user_id, std::int64_t request_id) const;

private:
    const repositories::AdoptRequestRepository& repository_;
    const AnimalService& animal_service_;
    const UserService& user_service_;
};

}  // namespace pawspective::services
