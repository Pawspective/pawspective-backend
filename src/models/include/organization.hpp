#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <tuple>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/formats/serialize/to.hpp>
#include "../../dto/include/organization_dto.hpp"
#include "../../dto/include/organization_register_dto.hpp"
#include "../../dto/include/organization_update_dto.hpp"

namespace pawspective::models {

struct Organization {
    static Organization from_register_dto(const dto::OrganizationRegisterDTO& reg);
    static Organization from_update_dto(const dto::OrganizationUpdateDTO& upd);
    static dto::OrganizationDTO to_dto(const Organization& model);
    std::int64_t id{};                       // NOLINT
    std::string name;                        // NOLINT
    std::optional<std::string> description;  // NOLINT
    // std::optional<std::string> avatar_url;       // NOLINT
    dto::City city;  // NOLINT

    auto introspect() const { return std::tie(id, name, description, city); }
};

}  // namespace pawspective::models
