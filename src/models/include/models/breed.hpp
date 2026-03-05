#pragma once

#include <cstdint>
#include <string>
#include <tuple>

#include <userver/formats/json/value_builder.hpp>
#include <userver/storages/postgres/io/io_fwd.hpp>
#include <userver/storages/postgres/io/pg_types.hpp>

namespace pawspective::models {

enum class AnimalType : std::uint8_t { kDog, kCat, kOther };

struct Breed {
    std::int64_t id;
    AnimalType animal_type;
    std::string name;

    auto introspect() const { return std::tie(id, animal_type, name); }
};

userver::formats::json::Value
Serialize(AnimalType value, userver::formats::serialize::To<userver::formats::json::Value>);

AnimalType Parse(const userver::formats::json::Value& value, userver::formats::parse::To<AnimalType>);

}  // namespace pawspective::models

namespace userver::storages::postgres::io {

template <>
struct CppToUserPg<pawspective::models::AnimalType> {
    static constexpr userver::storages::postgres::DBTypeName postgres_name = "animal_type";
    static constexpr auto enumerators = [](auto selector) {
        return selector()
            .Case("dog", pawspective::models::AnimalType::kDog)
            .Case("cat", pawspective::models::AnimalType::kCat)
            .Case("other", pawspective::models::AnimalType::kOther);
    };
};

}  // namespace userver::storages::postgres::io
