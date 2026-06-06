#pragma once

#include <string>
#include <userver/formats/json/value.hpp>
#include <vector>

namespace pawspective::dto {

struct PostCreateDTO {
    std::string text;
    std::vector<std::string> photos;
};

userver::formats::json::Value
Serialize(const PostCreateDTO& data, userver::formats::serialize::To<userver::formats::json::Value>);
PostCreateDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<PostCreateDTO>);

}  // namespace pawspective::dto
