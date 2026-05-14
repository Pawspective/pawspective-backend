#include <review_list_dto.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>  // NOLINT

namespace pawspective::dto {
userver::formats::json::Value
Serialize(const ReviewListDTO& review_list, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;
    builder["items"] = review_list.items;
    builder["page"] = review_list.page;
    builder["limit"] = review_list.limit;
    builder["total_count"] = review_list.total_count;
    builder["total_pages"] = review_list.total_pages;
    return builder.ExtractValue();
}
}  // namespace pawspective::dto
