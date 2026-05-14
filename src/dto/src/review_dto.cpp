#include "review_dto.hpp"

#include <userver/formats/json/value_builder.hpp>
#include <userver/utils/datetime.hpp>

namespace pawspective::dto {

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
