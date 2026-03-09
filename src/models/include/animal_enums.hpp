#pragma once

#include <cstdint>
#include <userver/formats/json/value_builder.hpp>
#include <userver/storages/postgres/io/enum_types.hpp>
#include <userver/storages/postgres/io/io_fwd.hpp>
#include <userver/storages/postgres/io/pg_types.hpp>

namespace pawspective::models {

enum class AnimalSize : std::uint8_t { kUnspecified, kSmall, kMedium, kLarge };

enum class AnimalGender : std::uint8_t { kUnspecified, kMale, kFemale, kUnknown };

enum class CareLevel : std::uint8_t { kUnspecified, kEasy, kModerate, kDifficult, kSpecialNeeds };

enum class GoodWith : std::uint8_t { kUnspecified, kDogs, kCats, kChildren, kElderly };

enum class AnimalColor : std::uint8_t {
    kUnspecified,
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

enum class AnimalStatus : std::uint8_t { kUnspecified, kAvailable, kAdopted, kUnavailable };

enum class AnimalType : std::uint8_t { kUnspecified, kDog, kCat, kOther };

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

userver::formats::json::Value
Serialize(AnimalType value, userver::formats::serialize::To<userver::formats::json::Value>);

AnimalType Parse(const userver::formats::json::Value& value, userver::formats::parse::To<AnimalType>);

}  // namespace pawspective::models

namespace userver::storages::postgres::io {

template <>
struct CppToUserPg<pawspective::models::AnimalSize> : EnumMappingBase<pawspective::models::AnimalSize> {
    static constexpr DBTypeName postgres_name = "animal_size";
    static constexpr EnumeratorList enumerators{
        {pawspective::models::AnimalSize::kSmall, "small"},
        {pawspective::models::AnimalSize::kMedium, "medium"},
        {pawspective::models::AnimalSize::kLarge, "large"}
    };
};

template <>
struct CppToUserPg<pawspective::models::AnimalGender> : EnumMappingBase<pawspective::models::AnimalGender> {
    static constexpr DBTypeName postgres_name = "animal_gender";
    static constexpr EnumeratorList enumerators{
        {pawspective::models::AnimalGender::kMale, "male"},
        {pawspective::models::AnimalGender::kFemale, "female"},
        {pawspective::models::AnimalGender::kUnknown, "unknown"}
    };
};

template <>
struct CppToUserPg<pawspective::models::CareLevel> : EnumMappingBase<pawspective::models::CareLevel> {
    static constexpr DBTypeName postgres_name = "care_level";
    static constexpr EnumeratorList enumerators{
        {pawspective::models::CareLevel::kEasy, "easy"},
        {pawspective::models::CareLevel::kModerate, "moderate"},
        {pawspective::models::CareLevel::kDifficult, "difficult"},
        {pawspective::models::CareLevel::kSpecialNeeds, "special_needs"}
    };
};

template <>
struct CppToUserPg<pawspective::models::GoodWith> : EnumMappingBase<pawspective::models::GoodWith> {
    static constexpr DBTypeName postgres_name = "good_with";
    static constexpr EnumeratorList enumerators{
        {pawspective::models::GoodWith::kDogs, "dogs"},
        {pawspective::models::GoodWith::kCats, "cats"},
        {pawspective::models::GoodWith::kChildren, "children"},
        {pawspective::models::GoodWith::kElderly, "elderly"}
    };
};

template <>
struct CppToUserPg<pawspective::models::AnimalColor> : EnumMappingBase<pawspective::models::AnimalColor> {
    static constexpr DBTypeName postgres_name = "animal_color";
    static constexpr EnumeratorList enumerators{
        {pawspective::models::AnimalColor::kBlack, "black"},
        {pawspective::models::AnimalColor::kWhite, "white"},
        {pawspective::models::AnimalColor::kBrown, "brown"},
        {pawspective::models::AnimalColor::kGrey, "grey"},
        {pawspective::models::AnimalColor::kOrange, "orange"},
        {pawspective::models::AnimalColor::kCream, "cream"},
        {pawspective::models::AnimalColor::kTan, "tan"},
        {pawspective::models::AnimalColor::kGolden, "golden"},
        {pawspective::models::AnimalColor::kSpotted, "spotted"},
        {pawspective::models::AnimalColor::kStriped, "striped"},
        {pawspective::models::AnimalColor::kBrindle, "brindle"},
        {pawspective::models::AnimalColor::kMixed, "mixed"}
    };
};

template <>
struct CppToUserPg<pawspective::models::AnimalStatus> : EnumMappingBase<pawspective::models::AnimalStatus> {
    static constexpr DBTypeName postgres_name = "animal_status";
    static constexpr EnumeratorList enumerators{
        {pawspective::models::AnimalStatus::kAvailable, "available"},
        {pawspective::models::AnimalStatus::kAdopted, "adopted"},
        {pawspective::models::AnimalStatus::kUnavailable, "unavailable"}
    };
};

template <>
struct CppToUserPg<pawspective::models::AnimalType> : EnumMappingBase<pawspective::models::AnimalType> {
    static constexpr DBTypeName postgres_name = "animal_type";
    static constexpr EnumeratorList enumerators{
        {pawspective::models::AnimalType::kDog, "dog"},
        {pawspective::models::AnimalType::kCat, "cat"},
        {pawspective::models::AnimalType::kOther, "other"}
    };
};

}  // namespace userver::storages::postgres::io
