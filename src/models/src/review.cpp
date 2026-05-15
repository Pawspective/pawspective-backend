#include "review.hpp"

namespace pawspective::models {
Review Review::from_create_dto(const dto::ReviewCreateDTO& create, std::int64_t user_id) {
    Review review;
    review.animal_id = create.animal_id;
    review.user_id = user_id;
    review.text = create.text;
    return review;
}

Review Review::from_update_dto(const dto::ReviewUpdateDTO& update, const std::int64_t review_id) {
    Review review;
    review.id = review_id;
    review.text = update.text.value_or("");
    return review;
}

dto::ReviewDTO Review::to_dto(const Review& model, const std::string& animal_name, bool can_edit) {
    dto::ReviewDTO review_dto;
    review_dto.id = model.id;
    review_dto.animal_id = model.animal_id;
    review_dto.animal_name = animal_name;
    review_dto.text = model.text;
    review_dto.created_at = model.created_at;
    review_dto.can_edit = can_edit;
    return review_dto;
}
}  // namespace pawspective::models
