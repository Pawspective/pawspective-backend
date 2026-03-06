#include "models/animal.hpp"

#include <userver/formats/json/value_builder.hpp>
#include "dto/animal_register.hpp"
#include "dto/animal_update.hpp"
#include "dto/animal.hpp"

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

AnimalUpdate::AnimalUpdate(const dto::AnimalUpdateDTO& upd)
    : name(upd.name),
      breed_id(upd.breed_id),
      size(upd.size),
      gender(upd.gender),
      care_level(upd.care_level),
      color(upd.color),
      good_with(upd.good_with),
      age(upd.age),
      description_updated(upd.description_updated),
      description(upd.description),
      status(upd.status) {}

}  // namespace pawspective::models
