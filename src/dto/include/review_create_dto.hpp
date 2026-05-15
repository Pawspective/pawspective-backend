#pragma once

#include <cstdint>
#include <string>
#include <userver/formats/json/value.hpp>

namespace pawspective::dto {

struct ReviewCreateDTO {
    std::int64_t animal_id{};
    std::string text;
};

ReviewCreateDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<ReviewCreateDTO>);

userver::formats::json::Value
Serialize(const ReviewCreateDTO& review_create, userver::formats::serialize::To<userver::formats::json::Value>);
}  // namespace pawspective::dto