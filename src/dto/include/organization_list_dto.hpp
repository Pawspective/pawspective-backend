#pragma once

#include <vector>

#include <userver/formats/json/value.hpp>
#include <userver/formats/serialize/to.hpp>

#include "organization_dto.hpp"

namespace pawspective::dto {

struct OrganizationListDTO {
    std::vector<OrganizationDTO> items;
    int page{};
    int limit{};
    std::int64_t total_count{};
    std::int64_t total_pages{};
};

userver::formats::json::Value
Serialize(const OrganizationListDTO& data, userver::formats::serialize::To<userver::formats::json::Value>);

}  // namespace pawspective::dto
