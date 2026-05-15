#include "adopt_request.hpp"

namespace pawspective::models {

dto::AdoptRequestDTO AdoptRequest::to_dto(
    const AdoptRequest& model,
    const std::string& email,
    const dto::AnimalDTO& animal
) {
    dto::AdoptRequestDTO dto;

    dto.id = model.id;
    dto.email = email;
    dto.animal = animal;

    return dto;
}

}  // namespace pawspective::models
