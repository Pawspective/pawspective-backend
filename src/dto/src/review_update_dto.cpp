#include "review_update_dto.hpp"
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/parse/common_containers.hpp>

namespace pawspective::dto {
ReviewUpdateDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<ReviewUpdateDTO>) {
    ReviewUpdateDTO dto;
    if (json.HasMember("text")) {
        dto.text = json["text"].As<std::optional<std::string>>();
    }
    return dto;
}


userver::formats::json::Value
Serialize(const ReviewUpdateDTO& review_update, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;
    if (review_update.text.has_value()) {
        builder["text"] = *review_update.text;
    }
    return builder.ExtractValue();
}
}  // namespace pawspective::dto
