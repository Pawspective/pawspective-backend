#pragma once
#include <cstdint>
#include <vector>
#include "review_dto.hpp"

namespace pawspective::dto {
struct ReviewListDTO {
    std::vector<ReviewDTO> items;
    int page{};
    int limit{};
    std::int64_t total_count{};
    std::int64_t total_pages{};
};

userver::formats::json::Value
Serialize(const ReviewListDTO& review_list, userver::formats::serialize::To<userver::formats::json::Value>);
}  // namespace pawspective::dto
