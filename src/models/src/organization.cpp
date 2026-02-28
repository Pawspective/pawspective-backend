#include "../include/organization.hpp"
#include <optional>
#include <userver/formats/json/value_builder.hpp>
#include "../../dto/include/organization_dto.hpp"
#include "../../dto/include/organization_register_dto.hpp"
#include "../../dto/include/organization_update_dto.hpp"

namespace pawspective::models {

Organization Organization::from_register_dto(const dto::OrganizationRegisterDTO& reg) {
    Organization org;

    org.name = reg.name;
    if (reg.description.has_value()) {
        org.description = *reg.description;
    }
    org.city = reg.city;

    return org;
}

Organization Organization::from_update_dto(const dto::OrganizationUpdateDTO& upd) {
    Organization org;

    if (upd.name.has_value()) {
        org.name = *upd.name;
    }
    if (upd.description.has_value()) {
        org.description = *upd.description;
    }
    if (upd.city.has_value()) {
        org.city = *upd.city;
    }

    return org;
}

dto::OrganizationDTO Organization::to_dto(const Organization& model) {
    dto::OrganizationDTO dto;

    dto.id = model.id;
    dto.name = model.name;
    dto.description = model.description;
    dto.city = model.city;

    return dto;
}

}  // namespace pawspective::models