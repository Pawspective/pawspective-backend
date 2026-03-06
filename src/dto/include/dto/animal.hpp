#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <userver/formats/json/value.hpp>
#include "dto/breed.hpp"
#include "models/animal.hpp"

namespace pawspective::dto {

struct AnimalDTO {
    // static Animal from_dto(const dto::AnimalDTO& dto);
    // static dto::AnimalDTO to_dto(const Animal& model);

    std::int64_t id{};
    std::int64_t organization_id{};
    std::string name;
    // std::optional<std::string> photo_url;
    BreedDTO breed;
    models::AnimalSize size = models::AnimalSize::kMedium;
    models::AnimalGender gender = models::AnimalGender::kUnknown;
    models::CareLevel care_level = models::CareLevel::kEasy;
    models::AnimalColor color = models::AnimalColor::kMixed;
    models::GoodWith good_with = models::GoodWith::kChildren;
    std::int32_t age{};
    std::optional<std::string> description;
    models::AnimalStatus status = models::AnimalStatus::kAvailable;
};

userver::formats::json::Value
Serialize(const AnimalDTO& data, userver::formats::serialize::To<userver::formats::json::Value>);
AnimalDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<AnimalDTO>);

struct AnimalUpdateDTO {
    std::optional<std::string> name;
    // std::optional<std::string> photo_url;
    std::optional<std::int64_t> breed_id;
    std::optional<models::AnimalSize> size;
    std::optional<models::AnimalGender> gender;
    std::optional<models::CareLevel> care_level;
    std::optional<models::AnimalColor> color;
    std::optional<models::GoodWith> good_with;
    std::optional<std::int32_t> age{};
    bool description_updated = false;
    std::optional<std::string> description;
    std::optional<models::AnimalStatus> status;
};

userver::formats::json::Value
Serialize(const AnimalUpdateDTO& data, userver::formats::serialize::To<userver::formats::json::Value>);
AnimalUpdateDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<AnimalUpdateDTO>);

struct AnimalRegisterDTO {
    std::int64_t organization_id{};
    std::string name;
    // std::optional<std::string> photo_url;
    std::int64_t breed_id{};
    models::AnimalSize size = models::AnimalSize::kMedium;
    models::AnimalGender gender = models::AnimalGender::kUnknown;
    models::CareLevel care_level = models::CareLevel::kEasy;
    models::AnimalColor color = models::AnimalColor::kMixed;
    models::GoodWith good_with = models::GoodWith::kChildren;
    std::int32_t age{};
    std::optional<std::string> description;
    models::AnimalStatus status = models::AnimalStatus::kAvailable;
};

userver::formats::json::Value
Serialize(const AnimalRegisterDTO& data, userver::formats::serialize::To<userver::formats::json::Value>);
AnimalRegisterDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<AnimalRegisterDTO>);

}  // namespace pawspective::dto
