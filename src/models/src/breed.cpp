#include "models/breed.hpp"

namespace pawspective::models {

    dto::BreedDTO Breed::ToDTO(const Breed& breed) {
        return dto::BreedDTO(
            breed.id,
            breed.animal_type,
            breed.name
        );
    }

} // namespace pawspective::models