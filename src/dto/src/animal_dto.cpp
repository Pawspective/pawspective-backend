#include "animal_dto.hpp"
#include <userver/formats/parse/common_containers.hpp>
#include <userver/formats/serialize/common_containers.hpp>

namespace pawspective::dto {

userver::formats::json::Value
Serialize(const AnimalDTO& data, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;

    builder["id"] = data.id;
    builder["organization_id"] = data.organization_id;
    builder["name"] = data.name;
    // builder["photo_url"] = data.photo_url; --- IGNORE ---
    builder["breed"] = data.breed;
    builder["size"] = data.size;
    builder["gender"] = data.gender;
    builder["care_level"] = data.care_level;
    builder["color"] = data.color;
    builder["good_with"] = data.good_with;
    builder["age"] = data.age;
    builder["description"] = data.description;
    builder["status"] = data.status;
    builder["user_id"] = data.user_id;

    return builder.ExtractValue();
}

AnimalDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<AnimalDTO>) {
    AnimalDTO dto;

    dto.id = json["id"].As<std::int64_t>();
    dto.organization_id = json["organization_id"].As<std::int64_t>();
    dto.name = json["name"].As<std::string>();
    dto.breed = json["breed"].As<BreedDTO>();
    dto.size = json["size"].As<pawspective::models::AnimalSize>();
    dto.gender = json["gender"].As<pawspective::models::AnimalGender>();
    dto.care_level = json["care_level"].As<pawspective::models::CareLevel>();
    dto.color = json["color"].As<pawspective::models::AnimalColor>();
    dto.good_with = json["good_with"].As<pawspective::models::GoodWith>();
    dto.age = json["age"].As<std::int32_t>();
    dto.description = json["description"].As<std::optional<std::string>>();
    dto.status = json["status"].As<pawspective::models::AnimalStatus>();
    dto.user_id = json["user_id"].As<std::optional<std::int64_t>>();

    return dto;
}

}  // namespace pawspective::dto
