#pragma once

#include <cstdint>
#include <string>
#include <userver/formats/json/value.hpp>

namespace pawspective::dto {

struct PostDTO {
    std::int64_t id{};
    std::int64_t organization_id{};
    std::string text;
    std::vector<std::string> photos;
    std::chrono::system_clock::time_point created_at{};
};

userver::formats::json::Value
Serialize(const PostDTO& data, userver::formats::serialize::To<userver::formats::json::Value>);
PostDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<PostDTO>);

}  // namespace pawspective::dto
