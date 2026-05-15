#include "../include/post.hpp"
#include <optional>

namespace pawspective::models {

Post Post::from_create_dto(std::int64_t organization_id, const dto::PostCreateDTO& create) {
    Post post;

    post.text = create.text;
    post.organization_id = organization_id;
    return post;
}

Post Post::from_update_dto(std::int64_t id, const dto::PostUpdateDTO& upd) {
    Post post;

    post.id = id;
    if (upd.text.has_value()) {
        post.text = *upd.text;
    }
    return post;
}

dto::PostDTO Post::to_dto(const Post& model) {
    dto::PostDTO dto;

    dto.id = model.id;
    dto.text = model.text;
    dto.organization_id = model.organization_id;
    dto.created_at = model.created_at;

    return dto;
}

}  // namespace pawspective::models