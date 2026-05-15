#include "review_dto.hpp"

#include <userver/formats/json/value_builder.hpp>
#include <userver/utils/datetime.hpp>

namespace pawspective::dto {


ReviewDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<ReviewDTO>) {
    ReviewDTO review;
    review.id = json["id"].As<std::int64_t>();
    review.animal_id = json["animal_id"].As<std::int64_t>();
    review.animal_name = json["animal_name"].As<std::string>();
    review.text = json["text"].As<std::string>();
    review.created_at = json["created_at"].As<std::chrono::system_clock::time_point>();
    review.can_edit = json["can_edit"].As<bool>();
    return review;
}

userver::formats::json::Value
Serialize(const ReviewDTO& review, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;
    builder["id"] = review.id;
    builder["animal_id"] = review.animal_id;
    builder["animal_name"] = review.animal_name;
    builder["text"] = review.text;
    builder["created_at"] = review.created_at;
    builder["can_edit"] = review.can_edit;
    return builder.ExtractValue();
}

}  // namespace pawspective::dto
