#pragma once

#include "animal_service.hpp"
#include "review_list_dto.hpp"
#include "review_repository.hpp"

namespace pawspective::services {

class ReviewService {
public:
    ReviewService(
        const repositories::ReviewRepository& review_repository,
        const AnimalService& animal_service,
        const OrganizationService& organization_service
    );

    [[nodiscard]] dto::ReviewListDTO GetByOrganizationIdPaginated(
        int64_t organization_id,
        int page,
        std::optional<int64_t> user_id = std::nullopt
    ) const;
    [[nodiscard]] bool ExistsUserReviewForAnimal(int64_t user_id, int64_t animal_id) const;
    dto::ReviewDTO Create(int64_t user_id, const dto::ReviewCreateDTO& review_create_dto) const;
    dto::ReviewDTO UpdateText(int64_t user_id, int64_t review_id, const dto::ReviewUpdateDTO& review_update_dto) const;
    void Delete(int64_t user_id, int64_t review_id) const;

private:
    const repositories::ReviewRepository& review_repository_;
    const AnimalService& animal_service_;
    const OrganizationService& organization_service_;
};

}  // namespace pawspective::services
