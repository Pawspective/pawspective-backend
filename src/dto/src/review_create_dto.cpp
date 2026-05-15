#include "review_create_dto.hpp"
#include <userver/formats/json/value_builder.hpp>

namespace pawspective::dto {

ReviewCreateDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<ReviewCreateDTO>) {
    ReviewCreateDTO review_create_dto;
    review_create_dto.animal_id = json["animal_id"].As<std::int64_t>();
    review_create_dto.text = json["text"].As<std::string>();
    return review_create_dto;
}

userver::formats::json::Value
Serialize(const ReviewCreateDTO& review_create, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;
    builder["animal_id"] = review_create.animal_id;
    builder["text"] = review_create.text;
    return builder.ExtractValue();
}
}  // namespace pawspective::dto
