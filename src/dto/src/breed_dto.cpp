#include "breed_dto.hpp"

namespace pawspective::dto {
userver::formats::json::Value
Serialize(const BreedDTO& data, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;

    builder["id"] = data.id;
    builder["animal_type"] = data.animal_type;
    builder["name"] = data.name;

    return builder.ExtractValue();
}

BreedDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<BreedDTO>) {
    BreedDTO dto;

    dto.id = json["id"].As<std::int64_t>();
    dto.animal_type = json["animal_type"].As<pawspective::models::AnimalType>();
    dto.name = json["name"].As<std::string>();

    return dto;
}

}  // namespace pawspective::dto
