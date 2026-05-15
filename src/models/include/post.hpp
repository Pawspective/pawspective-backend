#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include "../../dto/include/post_create_dto.hpp"
#include "../../dto/include/post_dto.hpp"
#include "../../dto/include/post_update_dto.hpp"

namespace pawspective::models {

struct Post {
    static Post from_create_dto(std::int64_t organization_id, const dto::PostCreateDTO& create);
    static Post from_update_dto(std::int64_t id, const dto::PostUpdateDTO& upd);
    static dto::PostDTO to_dto(const Post& model);

    std::int64_t id{};
    std::int64_t organization_id{};
    std::string text;
    // std::optional<std::string> photo_url;
    std::chrono::system_clock::time_point created_at{};
};

}  // namespace pawspective::models
