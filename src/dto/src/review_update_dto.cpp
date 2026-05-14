#include <review_update_dto.hpp>

namespace pawspective::dto {
ReviewUpdateDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<ReviewUpdateDTO>) {
    ReviewUpdateDTO review_update;
    review_update.text = json["text"].As<std::string>();
    return review_update;
}
}  // namespace pawspective::dto
