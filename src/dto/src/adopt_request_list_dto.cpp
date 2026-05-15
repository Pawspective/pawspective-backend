// cppcheck-suppress-file style
#include "../include/adopt_request_list_dto.hpp"

#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>  // NOLINT

namespace pawspective::dto {

userver::formats::json::Value
Serialize(const AdoptRequestListDTO& data, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;

    builder["items"] = data.items;
    builder["page"] = data.page;
    builder["limit"] = data.limit;
    builder["total_count"] = data.total_count;
    builder["total_pages"] = data.total_pages;

    return builder.ExtractValue();
}

}  // namespace pawspective::dto
