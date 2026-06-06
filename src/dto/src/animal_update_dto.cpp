#include "animal_update_dto.hpp"

#include <userver/formats/parse/common_containers.hpp>
#include <userver/formats/serialize/common_containers.hpp>

namespace pawspective::dto {

AnimalUpdateDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<AnimalUpdateDTO>) {
    AnimalUpdateDTO dto;

    dto.name = json["name"].As<std::optional<std::string>>();
    dto.breed_id = json["breed_id"].As<std::optional<std::int64_t>>();
    dto.photos = json["photos"].As<std::optional<std::vector<std::string>>>();
    dto.size = json["size"].As<std::optional<pawspective::models::AnimalSize>>();
    dto.gender = json["gender"].As<std::optional<pawspective::models::AnimalGender>>();
    dto.care_level = json["care_level"].As<std::optional<pawspective::models::CareLevel>>();
    dto.color = json["color"].As<std::optional<pawspective::models::AnimalColor>>();
    dto.good_with = json["good_with"].As<std::optional<pawspective::models::GoodWith>>();
    dto.age = json["age"].As<std::optional<std::int32_t>>();
    if (json.HasMember("description")) {
        dto.description = json["description"].As<std::optional<std::string>>();
    }
    dto.status = json["status"].As<std::optional<pawspective::models::AnimalStatus>>();
    return dto;
}

userver::formats::json::Value
Serialize(const AnimalUpdateDTO& data, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;
    builder["name"] = data.name;
    builder["breed_id"] = data.breed_id;
    builder["size"] = data.size;
    builder["gender"] = data.gender;
    builder["care_level"] = data.care_level;
    builder["color"] = data.color;
    builder["good_with"] = data.good_with;
    builder["age"] = data.age;
    builder["description"] = data.description;
    builder["status"] = data.status;
    builder["photos"] = data.photos;
    return builder.ExtractValue();
}

}  // namespace pawspective::dto
