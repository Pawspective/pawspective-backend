#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <tuple>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/formats/serialize/to.hpp>
#include "breed_dto.hpp"
#include "animal_enums.hpp"

#include "animal_fwd_dto.hpp"

namespace pawspective::models {

struct Animal {
    static Animal from_register_dto(const dto::AnimalRegisterDTO& reg);
    static Animal from_update_dto(std::int64_t id, const dto::AnimalUpdateDTO& upd);
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

}  // namespace pawspective::models
