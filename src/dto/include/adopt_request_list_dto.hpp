#pragma once

#include <vector>

#include <userver/formats/json/value.hpp>
#include <userver/formats/serialize/to.hpp>

#include "adopt_request_dto.hpp"

namespace pawspective::dto {

struct AdoptRequestListDTO {
    std::vector<AdoptRequestDTO> items;
    int page{};
    int limit{};
    std::int64_t total_count{};
    std::int64_t total_pages{};
};

userver::formats::json::Value
Serialize(const AdoptRequestListDTO& data, userver::formats::serialize::To<userver::formats::json::Value>);

}  // namespace pawspective::dto
