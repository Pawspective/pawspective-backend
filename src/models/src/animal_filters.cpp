#include "animal_filters.hpp"

namespace pawspective::models {

AnimalFilters AnimalFilters::from_dto(const dto::AnimalFilterDTO& dto) {
    AnimalFilters model;
    model.breed_ids = dto.breeds.value_or(std::vector<std::int64_t>{});
    model.animal_types = dto.animal_types.value_or(std::vector<AnimalType>{});
    model.sizes = dto.sizes.value_or(std::vector<AnimalSize>{});
    model.genders = dto.genders.value_or(std::vector<AnimalGender>{});
    model.care_levels = dto.care_levels.value_or(std::vector<CareLevel>{});
    model.colors = dto.colors.value_or(std::vector<AnimalColor>{});
    model.good_withs = dto.good_withs.value_or(std::vector<GoodWith>{});

    model.min_age = dto.age_lte.value_or(0);
    model.max_age = dto.age_gte.value_or(100);

    return model;
}

dto::AnimalFilterDTO AnimalFilters::to_dto(const AnimalFilters& model) {
    dto::AnimalFilterDTO dto;
    auto filter_unspecified = [](auto vec) {
        using T = typename decltype(vec)::value_type;
        vec.erase(std::remove(vec.begin(), vec.end(), T::kUnspecified), vec.end());
        return vec;
    };
    dto.breeds = model.breed_ids;
    dto.animal_types = filter_unspecified(model.animal_types);
    dto.sizes = filter_unspecified(model.sizes);
    dto.genders = filter_unspecified(model.genders);
    dto.care_levels = filter_unspecified(model.care_levels);
    dto.colors = filter_unspecified(model.colors);
    dto.good_withs = filter_unspecified(model.good_withs);

    dto.age_lte = model.min_age;
    dto.age_gte = model.max_age;

    return dto;
}

}  // namespace pawspective::models