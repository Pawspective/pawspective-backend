#pragma once

#include <optional>
#include <string>
#include <userver/formats/json/value.hpp>

namespace pawspective::dto {

struct PostUpdateDTO {
    std::optional<std::string> text;
    std::optional<std::vector<std::string>> photos;
};

userver::formats::json::Value
Serialize(const PostUpdateDTO& data, userver::formats::serialize::To<userver::formats::json::Value>);
PostUpdateDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<PostUpdateDTO>);

}  // namespace pawspective::dto
