#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <tuple>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/formats/serialize/to.hpp>
#include "organization_dto.hpp"
#include "organization_register_dto.hpp"
#include "organization_update_dto.hpp"

namespace pawspective::models {

struct Organization {
    static Organization from_register_dto(const dto::OrganizationRegisterDTO& reg);
    static dto::OrganizationDTO to_dto(const Organization& model, const dto::CityDTO& city_dto);
    std::int64_t id{};                       // NOLINT
    std::string name;                        // NOLINT
    std::optional<std::string> description;  // NOLINT
    // std::optional<std::string> avatar_url;       // NOLINT
    std::int64_t city_id{};  // NOLINT

    auto introspect() const { return std::tie(id, name, description, city_id); }
};

struct OrganizationUpdate {
    explicit OrganizationUpdate(std::int64_t id, const dto::OrganizationUpdateDTO& upd);
    std::int64_t id{};
    std::optional<std::string> name;
    bool description_updated = false;
    std::optional<std::string> description;
    // std::optional<std::string> avatar_url; // TODO: add avatar_url later
    std::optional<std::int64_t> city_id{};
};

}  // namespace pawspective::models
