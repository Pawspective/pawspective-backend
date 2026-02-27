#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <tuple>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/formats/serialize/to.hpp>
#include "../../dto/include/user_dto.hpp"
#include "../../dto/include/user_register_dto.hpp"
#include "../../dto/include/user_update_dto.hpp"

namespace pawspective::models {

struct User {
    static User from_register_dto(const dto::UserRegisterDTO& reg);
    static User from_update_dto(const dto::UserUpdateDTO& upd);
    static dto::UserDTO to_dto(const User& model);
    std::int64_t id{};                            // NOLINT
    std::string email;                            // NOLINT
    std::string first_name;                       // NOLINT
    std::string last_name;                        // NOLINT
    std::optional<std::int64_t> organization_id;  // NOLINT
    std::string password_hash;                    // NOLINT

    auto introspect() const { return std::tie(id, email, first_name, last_name, organization_id, password_hash); }
};

inline userver::formats::json::Value
Serialize(const User& user, userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;
    builder["id"] = user.id;
    builder["email"] = user.email;
    builder["first_name"] = user.first_name;
    builder["last_name"] = user.last_name;
    builder["organization_id"] = user.organization_id;
    builder["password_hash"] = user.password_hash;

    return builder.ExtractValue();
}

}  // namespace pawspective::models
