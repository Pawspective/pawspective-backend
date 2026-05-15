#include "post_create_dto.hpp"
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/parse/common_containers.hpp>
#include <userver/formats/serialize/common_containers.hpp>

namespace pawspective::dto {

userver::formats::json::Value
Serialize(const PostCreateDTO& data, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;

    builder["text"] = data.text;
    // builder["photo_url"] = data.photo_url;

    return builder.ExtractValue();
}

PostCreateDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<PostCreateDTO>) {
    PostCreateDTO dto;

    dto.text = json["text"].As<std::string>();
    // dto.photo_url = json["photo_url"].As<std::optional<std::string>>();

    return dto;
}

}  // namespace pawspective::dto
