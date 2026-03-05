#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <tuple>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/formats/serialize/to.hpp>
#include <userver/storages/postgres/io/io_fwd.hpp>
#include <userver/storages/postgres/io/pg_types.hpp>
#include "dto/breed.hpp"

#include "dto/animal_fwd.hpp"

namespace pawspective::models {

enum class AnimalSize : std::uint8_t { kSmall, kMedium, kLarge };

enum class AnimalGender : std::uint8_t { kMale, kFemale, kUnknown };

enum class CareLevel : std::uint8_t { kEasy, kModerate, kDifficult, kSpecialNeeds };

enum class GoodWith : std::uint8_t { kDogs, kCats, kChildren, kElderly };

enum class AnimalColor : std::uint8_t {
    kBlack,
    kWhite,
    kBrown,
    kGrey,
    kOrange,
    kCream,
    kTan,
    kGolden,
    kSpotted,
    kStriped,
    kBrindle,
    kMixed
};

enum class AnimalStatus : std::uint8_t { kAvailable, kAdopted, kUnavailable };

struct Animal {
    static Animal from_register_dto(const dto::AnimalRegisterDTO& reg);
    static dto::AnimalDTO to_dto(const Animal& model, const dto::BreedDTO& breed);

    std::int64_t id{};
    std::int64_t organization_id{};
    std::string name;
    // std::optional<std::string> photo_url;
    std::int64_t breed_id;
    AnimalSize size;
    AnimalGender gender;
    CareLevel care_level;
    AnimalColor color;
    GoodWith good_with;
    std::int32_t age{};
    std::optional<std::string> description;
    AnimalStatus status;

    auto introspect() const {
        return std::tie(
            id,
            organization_id,
            name /*, photo_url*/,
            breed_id,
            size,
            gender,
            care_level,
            good_with,
            color,
            age,
            description,
            status
        );
    }
};

struct AnimalUpdate {
    explicit AnimalUpdate(const dto::AnimalUpdateDTO& upd);

    std::optional<std::string> name;
    std::optional<std::int64_t> breed_id;
    std::optional<AnimalSize> size;
    std::optional<AnimalGender> gender;
    std::optional<CareLevel> care_level;
    std::optional<AnimalColor> color;
    std::optional<GoodWith> good_with;
    std::optional<std::int32_t> age;
    std::optional<std::string> description;
    std::optional<AnimalStatus> status;
};

// JSON Parse/Serialize declarations
userver::formats::json::Value
Serialize(AnimalSize value, userver::formats::serialize::To<userver::formats::json::Value>);

AnimalSize Parse(const userver::formats::json::Value& value, userver::formats::parse::To<AnimalSize>);

userver::formats::json::Value
Serialize(AnimalGender value, userver::formats::serialize::To<userver::formats::json::Value>);

AnimalGender Parse(const userver::formats::json::Value& value, userver::formats::parse::To<AnimalGender>);

userver::formats::json::Value
Serialize(CareLevel value, userver::formats::serialize::To<userver::formats::json::Value>);

CareLevel Parse(const userver::formats::json::Value& value, userver::formats::parse::To<CareLevel>);

userver::formats::json::Value Serialize(GoodWith value, userver::formats::serialize::To<userver::formats::json::Value>);

GoodWith Parse(const userver::formats::json::Value& value, userver::formats::parse::To<GoodWith>);

userver::formats::json::Value
Serialize(AnimalColor value, userver::formats::serialize::To<userver::formats::json::Value>);

AnimalColor Parse(const userver::formats::json::Value& value, userver::formats::parse::To<AnimalColor>);

userver::formats::json::Value
Serialize(AnimalStatus value, userver::formats::serialize::To<userver::formats::json::Value>);

AnimalStatus Parse(const userver::formats::json::Value& value, userver::formats::parse::To<AnimalStatus>);

}  // namespace pawspective::models

namespace userver::storages::postgres::io {

template <>
struct CppToUserPg<pawspective::models::AnimalSize> {
    static constexpr userver::storages::postgres::DBTypeName postgres_name = "animal_size";
    static constexpr auto enumerators = [](auto selector) {
        return selector()
            .Case("small", pawspective::models::AnimalSize::kSmall)
            .Case("medium", pawspective::models::AnimalSize::kMedium)
            .Case("large", pawspective::models::AnimalSize::kLarge);
    };
};

template <>
struct CppToUserPg<pawspective::models::AnimalGender> {
    static constexpr userver::storages::postgres::DBTypeName postgres_name = "animal_gender";
    static constexpr auto enumerators = [](auto selector) {
        return selector()
            .Case("male", pawspective::models::AnimalGender::kMale)
            .Case("female", pawspective::models::AnimalGender::kFemale)
            .Case("unknown", pawspective::models::AnimalGender::kUnknown);
    };
};

template <>
struct CppToUserPg<pawspective::models::CareLevel> {
    static constexpr userver::storages::postgres::DBTypeName postgres_name = "care_level";
    static constexpr auto enumerators = [](auto selector) {
        return selector()
            .Case("easy", pawspective::models::CareLevel::kEasy)
            .Case("moderate", pawspective::models::CareLevel::kModerate)
            .Case("difficult", pawspective::models::CareLevel::kDifficult)
            .Case("special_needs", pawspective::models::CareLevel::kSpecialNeeds);
    };
};

template <>
struct CppToUserPg<pawspective::models::GoodWith> {
    static constexpr userver::storages::postgres::DBTypeName postgres_name = "good_with";
    static constexpr auto enumerators = [](auto selector) {
        return selector()
            .Case("dogs", pawspective::models::GoodWith::kDogs)
            .Case("cats", pawspective::models::GoodWith::kCats)
            .Case("children", pawspective::models::GoodWith::kChildren)
            .Case("elderly", pawspective::models::GoodWith::kElderly);
    };
};

template <>
struct CppToUserPg<pawspective::models::AnimalColor> {
    static constexpr userver::storages::postgres::DBTypeName postgres_name = "animal_color";
    static constexpr auto enumerators = [](auto selector) {
        return selector()
            .Case("black", pawspective::models::AnimalColor::kBlack)
            .Case("white", pawspective::models::AnimalColor::kWhite)
            .Case("brown", pawspective::models::AnimalColor::kBrown)
            .Case("grey", pawspective::models::AnimalColor::kGrey)
            .Case("orange", pawspective::models::AnimalColor::kOrange)
            .Case("cream", pawspective::models::AnimalColor::kCream)
            .Case("tan", pawspective::models::AnimalColor::kTan)
            .Case("golden", pawspective::models::AnimalColor::kGolden)
            .Case("spotted", pawspective::models::AnimalColor::kSpotted)
            .Case("striped", pawspective::models::AnimalColor::kStriped)
            .Case("brindle", pawspective::models::AnimalColor::kBrindle)
            .Case("mixed", pawspective::models::AnimalColor::kMixed);
    };
};

template <>
struct CppToUserPg<pawspective::models::AnimalStatus> {
    static constexpr userver::storages::postgres::DBTypeName postgres_name = "animal_status";
    static constexpr auto enumerators = [](auto selector) {
        return selector()
            .Case("available", pawspective::models::AnimalStatus::kAvailable)
            .Case("adopted", pawspective::models::AnimalStatus::kAdopted)
            .Case("unavailable", pawspective::models::AnimalStatus::kUnavailable);
    };
};

}  // namespace userver::storages::postgres::io
