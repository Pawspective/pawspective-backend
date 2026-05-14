#include "review_create_dto.hpp"

namespace pawspective::dto {

ReviewCreateDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<ReviewCreateDTO>) {
    ReviewCreateDTO review_create_dto;
    review_create_dto.animal_id = json["animal_id"].As<std::int64_t>();
    review_create_dto.text = json["text"].As<std::string>();
    return review_create_dto;
}

}  // namespace pawspective::dto
