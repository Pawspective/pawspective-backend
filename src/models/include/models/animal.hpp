#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <tuple>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/formats/serialize/to.hpp>
#include "dto/breed.hpp"
#include "models/animal_enums.hpp"

#include "dto/animal_fwd.hpp"

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

// struct AnimalUpdate {
//     explicit AnimalUpdate(const dto::AnimalUpdateDTO& upd);
//     int64_t id{};
//     std::optional<std::string> name;
//     std::optional<std::int64_t> breed_id;
//     std::optional<AnimalSize> size;
//     std::optional<AnimalGender> gender;
//     std::optional<CareLevel> care_level;
//     std::optional<AnimalColor> color;
//     std::optional<GoodWith> good_with;
//     std::optional<std::int32_t> age;
//     bool description_updated = false;
//     std::optional<std::string> description;
//     std::optional<AnimalStatus> status;
// };

}  // namespace pawspective::models
