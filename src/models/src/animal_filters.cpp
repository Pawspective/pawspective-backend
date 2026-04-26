#include "animal_filters.hpp"

namespace pawspective::models {

AnimalFilters AnimalFilters::from_dto(const dto::AnimalFilterDTO& dto) {
    AnimalFilters model;
    model.breed_ids = dto.breeds.value_or(std::vector<std::int64_t>{});
    model.city_ids = dto.city_ids.value_or(std::vector<std::int64_t>{});
    model.animal_types = dto.animal_types.value_or(std::vector<AnimalType>{});
    model.sizes = dto.sizes.value_or(std::vector<AnimalSize>{});
    model.genders = dto.genders.value_or(std::vector<AnimalGender>{});
    model.care_levels = dto.care_levels.value_or(std::vector<CareLevel>{});
    model.colors = dto.colors.value_or(std::vector<AnimalColor>{});
    model.good_withs = dto.good_withs.value_or(std::vector<GoodWith>{});

    model.min_age = dto.age_gte.value_or(0);
    model.max_age = dto.age_lte.value_or(100);

    return model;
}

dto::AnimalFilterDTO AnimalFilters::to_dto(const AnimalFilters& model) {
    dto::AnimalFilterDTO dto;
    auto filter_unspecified = [](auto& vec) {
        using T = typename std::decay_t<decltype(vec)>::value_type;
        vec.erase(std::remove(vec.begin(), vec.end(), T::kUnspecified), vec.end());
    };
    dto.breeds = model.breed_ids;
    dto.city_ids = model.city_ids;
    dto.animal_types = model.animal_types;
    dto.sizes = model.sizes;
    dto.genders = model.genders;
    dto.care_levels = model.care_levels;
    dto.colors = model.colors;
    dto.good_withs = model.good_withs;

    if (dto.animal_types.has_value()) {
        filter_unspecified(dto.animal_types.value());
    }
    if (dto.sizes.has_value()) {
        filter_unspecified(dto.sizes.value());
    }
    if (dto.genders.has_value()) {
        filter_unspecified(dto.genders.value());
    }
    if (dto.care_levels.has_value()) {
        filter_unspecified(dto.care_levels.value());
    }
    if (dto.colors.has_value()) {
        filter_unspecified(dto.colors.value());
    }
    if (dto.good_withs.has_value()) {
        filter_unspecified(dto.good_withs.value());
    }

    dto.age_gte = model.min_age;
    dto.age_lte = model.max_age;

    return dto;
}

}  // namespace pawspective::models