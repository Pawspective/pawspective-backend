#include "review_service.hpp"

#include "services/exception.hpp"

namespace pawspective::services {

ReviewService::ReviewService(
    const repositories::ReviewRepository& review_repository,
    const AnimalService& animal_service,
    const OrganizationService& organization_service
)
    : review_repository_(review_repository),
      animal_service_(animal_service),
      organization_service_(organization_service) {}

[[nodiscard]] dto::ReviewListDTO ReviewService::GetByOrganizationIdPaginated(
    int64_t organization_id,
    int page,
    std::optional<int64_t> user_id
) const {
    // check on existing organization
    (void)organization_service_.Get(organization_id);
    static constexpr int kPageSize = 20;

    auto [reviews, total_count] = review_repository_.GetByOrganizationIdPaginated(organization_id, page, kPageSize);
    dto::ReviewListDTO review_list_dto;
    std::vector<int64_t> animal_ids;
    std::transform(reviews.begin(), reviews.end(), std::back_inserter(animal_ids), [](const models::Review& review) {
        return review.animal_id;
    });
    auto animal_names = animal_service_.GetNames(std::move(animal_ids));
    for (const auto& review : reviews) {
        review_list_dto.items.push_back(models::Review::to_dto(
            review,
            animal_names[review.animal_id],
            user_id.has_value() && review.user_id == user_id.value()
        ));
    }
    review_list_dto.page = page;
    review_list_dto.limit = kPageSize;
    review_list_dto.total_count = total_count;
    review_list_dto.total_pages = total_count == 0 ? 1 : (total_count + kPageSize - 1) / kPageSize;
    return review_list_dto;
}

dto::ReviewDTO ReviewService::Create(int64_t user_id, const dto::ReviewCreateDTO& review_create_dto) const {
    auto animal = animal_service_.Get(review_create_dto.animal_id);
    // check on existing organization
    (void)organization_service_.Get(animal.organization_id);

    if (review_repository_.ExistsUserReviewForAnimal(user_id, review_create_dto.animal_id)) {
        throw ReviewAlreadyExistsException();
    }
    models::Review review = models::Review::from_create_dto(review_create_dto, user_id);
    auto created_review = review_repository_.Create(review);
    return models::Review::to_dto(created_review, animal_service_.Get(created_review.animal_id).name, true);
}

dto::ReviewDTO ReviewService::UpdateText(
    int64_t user_id,
    int64_t review_id,
    const dto::ReviewUpdateDTO& review_update_dto
) const {
    auto review = review_repository_.GetById(review_id);
    if (!review) {
        throw ReviewNotFoundException();
    }
    if (review->user_id != user_id) {
        throw ForbiddenException();
    }
    if (!review_update_dto.text.has_value() || review_update_dto.text == review->text) {
        return models::Review::to_dto(*review, animal_service_.Get(review->animal_id).name, true);
    }
    auto updated_review = review_repository_.UpdateTextById(review_id, review_update_dto.text.value());
    if (!updated_review) {
        throw ReviewNotFoundException();
    }
    return models::Review::to_dto(*updated_review, animal_service_.Get(review->animal_id).name, true);
}

void ReviewService::Delete(int64_t user_id, int64_t review_id) const {
    auto review = review_repository_.GetById(review_id);
    if (!review) {
        throw ReviewNotFoundException();
    }
    if (review->user_id != user_id) {
        throw ForbiddenException();
    }
    if (!review_repository_.Delete(review_id)) {
        throw ReviewNotFoundException();
    }
}

}  // namespace pawspective::services
