#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <userver/formats/json/value.hpp>

namespace pawspective::dto {
struct ReviewDTO {
    std::int64_t id{};
    std::int64_t animal_id{};
    std::string animal_name;
    std::string text;
    std::chrono::system_clock::time_point created_at{};
    bool can_edit = false;
};

userver::formats::json::Value
Serialize(const ReviewDTO& review, userver::formats::serialize::To<userver::formats::json::Value>);
}  // namespace pawspective::dto
