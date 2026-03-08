#include "animal.hpp"

#include <userver/formats/json/value_builder.hpp>
#include "animal_dto.hpp"
#include "animal_register_dto.hpp"
#include "animal_update_dto.hpp"

namespace pawspective::models {

// cppcheck-suppress uninitvar
Animal Animal::from_register_dto(const dto::AnimalRegisterDTO& reg) {
    return Animal(
        -1,  // id will be set by the database
        reg.organization_id,
        reg.name,
        // reg.photo_url, --- IGNORE ---
        reg.breed_id,
        reg.size,
        reg.gender,
        reg.care_level,
        reg.color,
        reg.good_with,
        reg.age,
        reg.description,
        reg.status
    );
}

dto::AnimalDTO Animal::to_dto(const Animal& model, const dto::BreedDTO& breed) {
    dto::AnimalDTO dto;

    dto.id = model.id;
    dto.organization_id = model.organization_id;
    dto.name = model.name;
    // dto.photo_url = model.photo_url; --- IGNORE ---
    dto.breed = breed;
    dto.size = model.size;
    dto.gender = model.gender;
    dto.care_level = model.care_level;
    dto.color = model.color;
    dto.good_with = model.good_with;
    dto.age = model.age;
    dto.description = model.description;
    dto.status = model.status;

    return dto;
}

Animal Animal::from_update_dto(std::int64_t id, const dto::AnimalUpdateDTO& upd) {
    return Animal(
        id,
        -1,  // organization_id will not be updated
        upd.name.value_or(""),
        // std::nullopt, --- IGNORE ---
        upd.breed_id.value_or(-1),
        upd.size.value_or(AnimalSize::kUnspecified),
        upd.gender.value_or(AnimalGender::kUnspecified),
        upd.care_level.value_or(CareLevel::kUnspecified),
        upd.color.value_or(AnimalColor::kUnspecified),
        upd.good_with.value_or(GoodWith::kUnspecified),
        upd.age.value_or(-1),
        upd.description,
        upd.status.value_or(AnimalStatus::kUnspecified)
    );
}

}  // namespace pawspective::models
