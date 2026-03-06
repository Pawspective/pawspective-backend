#include "dto/animal_register.hpp"

#include <userver/formats/json/value.hpp>
#include <userver/formats/parse/common_containers.hpp>
#include <userver/formats/serialize/common_containers.hpp>

namespace pawspective::dto {

AnimalRegisterDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<AnimalRegisterDTO>) {
    AnimalRegisterDTO dto;

    dto.organization_id = json["organization_id"].As<std::int64_t>();
    dto.name = json["name"].As<std::string>();
    dto.breed_id = json["breed_id"].As<std::int64_t>();
    dto.size = json["size"].As<pawspective::models::AnimalSize>();
    dto.gender = json["gender"].As<pawspective::models::AnimalGender>();
    dto.care_level = json["care_level"].As<pawspective::models::CareLevel>();
    dto.color = json["color"].As<pawspective::models::AnimalColor>();
    dto.good_with = json["good_with"].As<pawspective::models::GoodWith>();
    dto.age = json["age"].As<std::int32_t>();
    dto.description = json["description"].As<std::optional<std::string>>();
    dto.status = json["status"].As<pawspective::models::AnimalStatus>();
    return dto;
}

userver::formats::json::Value
Serialize(const AnimalRegisterDTO& data, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;

    builder["organization_id"] = data.organization_id;
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

    return builder.ExtractValue();
}

} // namespace pawspective::dto
