#pragma once

#include <string>
#include <userver/formats/json/value.hpp>

namespace pawspective::dto {
struct ReviewUpdateDTO {
    std::string text;
};

ReviewUpdateDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<ReviewUpdateDTO>);
}  // namespace pawspective::dto
