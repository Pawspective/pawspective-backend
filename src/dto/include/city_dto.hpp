#pragma once
#include <cstdint>
#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/parse/to.hpp>
#include <userver/formats/serialize/to.hpp>

namespace pawspective::dto {

struct CityDTO {
    std::int64_t id{};
    std::string name;
};

userver::formats::json::Value
Serialize(const CityDTO& data, userver::formats::serialize::To<userver::formats::json::Value>);
CityDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To<CityDTO>);

}  // namespace pawspective::dto