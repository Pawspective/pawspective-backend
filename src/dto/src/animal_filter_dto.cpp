#include "animal_filter_dto.hpp"
#include <userver/formats/parse/common_containers.hpp>
#include <userver/formats/serialize/common_containers.hpp>

namespace pawspective::dto {

AnimalFilterDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<AnimalFilterDTO>) {
    AnimalFilterDTO dto;

    dto.breeds = json["breeds"].As<std::optional<std::vector<std::int64_t>>>();
    dto.city_ids = json["city_ids"].As<std::optional<std::vector<std::int64_t>>>();
    dto.animal_types = json["animal_types"].As<std::optional<std::vector<models::AnimalType>>>();
    dto.sizes = json["sizes"].As<std::optional<std::vector<models::AnimalSize>>>();
    dto.genders = json["genders"].As<std::optional<std::vector<models::AnimalGender>>>();
    dto.care_levels = json["care_levels"].As<std::optional<std::vector<models::CareLevel>>>();
    dto.colors = json["colors"].As<std::optional<std::vector<models::AnimalColor>>>();
    dto.good_withs = json["good_withs"].As<std::optional<std::vector<models::GoodWith>>>();
    dto.statuses = json["statuses"].As<std::optional<std::vector<models::AnimalStatus>>>();
    dto.age_lte = json["age_lte"].As<std::optional<int>>();
    dto.age_gte = json["age_gte"].As<std::optional<int>>();

    return dto;
}

userver::formats::json::Value
Serialize(const AnimalFilterDTO& filters, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;
    builder["breeds"] = filters.breeds;
    builder["cityIds"] = filters.city_ids;
    builder["animalTypes"] = filters.animal_types;
    builder["sizes"] = filters.sizes;
    builder["genders"] = filters.genders;
    builder["careLevels"] = filters.care_levels;
    builder["colors"] = filters.colors;
    builder["goodWiths"] = filters.good_withs;
    builder["ageLte"] = filters.age_lte;
    builder["ageGte"] = filters.age_gte;
    builder["statuses"] = filters.statuses;

    return builder.ExtractValue();
}

}  // namespace pawspective::dto