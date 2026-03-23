#pragma once

#include <cstdint>
#include <vector>

#include "../../dto/include/animal_filter_dto.hpp"
#include "animal_enums.hpp"

namespace pawspective::models {

struct AnimalFilters {
    static AnimalFilters from_dto(const dto::AnimalFilterDTO& dto);
    static dto::AnimalFilterDTO to_dto(const AnimalFilters& model);

    std::vector<std::int64_t> breed_ids;
    std::vector<AnimalType> animal_types;
    std::vector<AnimalSize> sizes;
    std::vector<AnimalGender> genders;
    std::vector<CareLevel> care_levels;
    std::vector<AnimalColor> colors;
    std::vector<GoodWith> good_withs;

    int min_age{0};
    int max_age{0};
};

}  // namespace pawspective::models