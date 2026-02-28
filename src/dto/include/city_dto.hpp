#pragma once
#include <cstdint>
#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/parse/to.hpp>
#include <userver/formats/serialize/to.hpp>

namespace pawspective::dto {

struct City {
    std::int64_t id{};
    std::string name;
};

userver::formats::json::Value
Serialize(const City& data, userver::formats::serialize::To<userver::formats::json::Value>);
City Parse(const userver::formats::json::Value& json, userver::formats::parse::To<City>);

}  // namespace pawspective::dto