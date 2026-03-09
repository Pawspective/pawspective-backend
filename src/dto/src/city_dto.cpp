// cppcheck-suppress-file style
#include "../include/city_dto.hpp"
#include <cstdint>
#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/parse/common_containers.hpp>  // NOLINT
#include <userver/formats/parse/to.hpp>
#include <userver/formats/serialize/common_containers.hpp>  // NOLINT
#include <userver/formats/serialize/to.hpp>

namespace pawspective::dto {

[[maybe_unused]] userver::formats::json::Value
Serialize(const CityDTO& data, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;

    builder["id"] = data.id;
    builder["name"] = data.name;

    return builder.ExtractValue();
}

[[maybe_unused]] CityDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<CityDTO>) {
    CityDTO city;

    city.id = json["id"].As<std::int64_t>();
    city.name = json["name"].As<std::string>();

    return city;
}

}  // namespace pawspective::dto
