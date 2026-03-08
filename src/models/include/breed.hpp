#pragma once

#include <cstdint>
#include <string>
#include <tuple>

#include <userver/formats/json/value_builder.hpp>
#include <userver/storages/postgres/io/io_fwd.hpp>
#include <userver/storages/postgres/io/pg_types.hpp>

#include "breed_dto.hpp"
#include "animal_enums.hpp"

namespace pawspective::models {

struct Breed {
    static dto::BreedDTO ToDTO(const Breed& breed);
    std::int64_t id;
    AnimalType animal_type;
    std::string name;

    auto introspect() const { return std::tie(id, animal_type, name); }
};

}  // namespace pawspective::models
