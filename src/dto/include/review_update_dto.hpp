#pragma once

#include <string>
#include <userver/formats/json/value.hpp>

namespace pawspective::dto {
struct ReviewUpdateDTO {
    std::optional<std::string> text;
};

ReviewUpdateDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<ReviewUpdateDTO>);

userver::formats::json::Value
Serialize(const ReviewUpdateDTO& review_update, userver::formats::serialize::To<userver::formats::json::Value>);
}  // namespace pawspective::dto
