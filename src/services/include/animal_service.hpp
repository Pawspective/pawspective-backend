#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "adopt_request_repository.hpp"
#include "animal_dto.hpp"
#include "animal_filter_dto.hpp"
#include "animal_list_dto.hpp"
#include "animal_register_dto.hpp"
#include "animal_repository.hpp"
#include "animal_update_dto.hpp"
#include "breed_service.hpp"
#include "organization_service.hpp"
#include "utils/s3_upload_client.hpp"

namespace pawspective::services {

class AnimalService {
public:
    explicit AnimalService(
        const repositories::AnimalRepository& repo,
        const BreedService& breed_service,
        const OrganizationService& org_service,
        const UserService& user_service,
        const repositories::AdoptRequestRepository& adopt_request_repo,
        utils::S3UploadClient& s3
    );

    dto::AnimalDTO Create(int64_t user_id, const dto::AnimalRegisterDTO& dto) const;
    dto::AnimalDTO Update(int64_t user_id, int64_t animal_id, const dto::AnimalUpdateDTO& dto) const;
    dto::AnimalDTO Adopt(int64_t user_id, int64_t animal_id) const;
    [[nodiscard]] dto::AnimalDTO Get(int64_t id, std::optional<int64_t> user_id = std::nullopt) const;
    [[nodiscard]] std::unordered_map<int64_t, std::string> GetNames(const std::vector<int64_t>& ids) const;
    [[nodiscard]] std::vector<dto::AnimalDTO> GetAdoptedAnimalsWithoutReviewsByUserId(int64_t user_id) const;
    [[nodiscard]] std::vector<dto::AnimalDTO> GetByIds(
        const std::vector<std::int64_t>& ids,
        std::optional<int64_t> user_id = std::nullopt
    ) const;
    [[nodiscard]] dto::AnimalListDTO GetByOrganizationPaginated(
        int64_t org_id,
        int page,
        std::optional<int64_t> user_id = std::nullopt
    ) const;
    [[nodiscard]] dto::AnimalFilterDTO GetFilterOptions() const;
    [[nodiscard]] dto::AnimalListDTO FindByFilters(
        const dto::AnimalFilterDTO& dto,
        int page,
        std::optional<int64_t> user_id = std::nullopt
    ) const;
    void Delete(int64_t user_id, int64_t animal_id) const;

private:
    const repositories::AnimalRepository& repository_;
    const BreedService& breed_service_;
    const OrganizationService& org_service_;
    const UserService& user_service_;
    const repositories::AdoptRequestRepository& adopt_request_repo_;
    utils::S3UploadClient& s3_;
};

}  // namespace pawspective::services