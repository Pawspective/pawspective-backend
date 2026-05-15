#pragma once

#include <string>
#include <userver/formats/json/value.hpp>

namespace pawspective::dto {

struct PostCreateDTO {
    std::string text;
    // std::optional<std::string> photo_url;
};

userver::formats::json::Value
Serialize(const PostCreateDTO& data, userver::formats::serialize::To<userver::formats::json::Value>);
PostCreateDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<PostCreateDTO>);

}  // namespace pawspective::dto
