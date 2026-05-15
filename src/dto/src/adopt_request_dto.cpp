// cppcheck-suppress-file style
#include "../include/adopt_request_dto.hpp"

#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/parse/common_containers.hpp>      // NOLINT
#include <userver/formats/serialize/common_containers.hpp>  // NOLINT

namespace pawspective::dto {

[[maybe_unused]] userver::formats::json::Value
Serialize(const AdoptRequestDTO& data, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;

    builder["id"] = data.id;
    builder["email"] = data.email;
    builder["animal"] = data.animal;

    return builder.ExtractValue();
}

[[maybe_unused]] AdoptRequestDTO
Parse(const userver::formats::json::Value& json, userver::formats::parse::To<AdoptRequestDTO>) {
    AdoptRequestDTO dto;

    dto.id = json["id"].As<std::int64_t>();
    dto.email = json["email"].As<std::string>();
    dto.animal = json["animal"].As<AnimalDTO>();

    return dto;
}

}  // namespace pawspective::dto
