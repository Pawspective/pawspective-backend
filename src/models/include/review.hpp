#pragma once

#include <chrono>
#include <string>
#include "review_create_dto.hpp"
#include "review_dto.hpp"
#include "review_update_dto.hpp"

namespace pawspective::models {
struct Review {
    static Review from_create_dto(const dto::ReviewCreateDTO& create, std::int64_t user_id);
    static Review from_update_dto(const dto::ReviewUpdateDTO& update, std::int64_t review_id);
    static dto::ReviewDTO to_dto(const Review& model, const std::string& animal_name, bool can_edit);
    std::int64_t id{};
    std::int64_t animal_id{};
    std::int64_t user_id{};
    std::string text;
    std::chrono::system_clock::time_point created_at;
};
}  // namespace pawspective::models