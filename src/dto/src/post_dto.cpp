#include "post_dto.hpp"
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/parse/common_containers.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/utils/datetime.hpp>

namespace pawspective::dto {

userver::formats::json::Value
Serialize(const PostDTO& data, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;

    builder["id"] = data.id;
    builder["organization_id"] = data.organization_id;
    builder["text"] = data.text;
    builder["photos"] = data.photos;
    builder["created_at"] = data.created_at;

    return builder.ExtractValue();
}

PostDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<PostDTO>) {
    PostDTO dto;

    dto.id = json["id"].As<std::int64_t>();
    dto.organization_id = json["organization_id"].As<std::int64_t>();
    dto.text = json["text"].As<std::string>();
    dto.photos = json["photos"].As<std::vector<std::string>>();
    dto.created_at = userver::utils::datetime::FromRfc3339StringSaturating(json["created_at"].As<std::string>());
    return dto;
}

}  // namespace pawspective::dto
