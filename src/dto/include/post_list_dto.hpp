#pragma once

#include <vector>

#include <userver/formats/json/value.hpp>
#include <userver/formats/serialize/to.hpp>

#include "post_dto.hpp"

namespace pawspective::dto {

struct PostListDTO {
    std::vector<PostDTO> items;
    int page{};
    int limit{};
    std::int64_t total_count{};
    std::int64_t total_pages{};
};

userver::formats::json::Value
Serialize(const PostListDTO& data, userver::formats::serialize::To<userver::formats::json::Value>);

}  // namespace pawspective::dto
