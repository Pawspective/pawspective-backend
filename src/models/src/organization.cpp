#include "models/organization.hpp"
#include <optional>
#include <userver/formats/json/value_builder.hpp>
#include "../../dto/include/organization_dto.hpp"
#include "../../dto/include/organization_register_dto.hpp"
#include "../../dto/include/organization_update_dto.hpp"

namespace pawspective::models {

Organization Organization::from_register_dto(const dto::OrganizationRegisterDTO& reg) {
    return Organization(
        -1,  // id will be set by the database
        reg.name,
        reg.description,
        reg.city_id
    );
}

dto::OrganizationDTO Organization::to_dto(const Organization& model, const dto::CityDTO& city_dto) {
    dto::OrganizationDTO dto;

    dto.id = model.id;
    dto.name = model.name;
    dto.description = model.description;
    dto.city = city_dto;

    return dto;
}

OrganizationUpdate::OrganizationUpdate(const std::int64_t id, const dto::OrganizationUpdateDTO& upd)
    : id(id),
      name(upd.name),
      description_updated(upd.description_updated),
      description(upd.description),
      city_id(upd.city_id) {}

}  // namespace pawspective::models