#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <tuple>
#include "../../dto/include/user_dto.hpp"
#include "../../dto/include/user_register_dto.hpp"
#include "../../dto/include/user_update_dto.hpp"

namespace pawspective::models {

struct User {
    static User from_register_dto(const dto::UserRegisterDTO &reg);
    static User from_update_dto(const dto::UserUpdateDTO &upd);
    static dto::UserDTO to_dto(const User &model);
    std::int64_t id{};                            // NOLINT
    std::string email;                            // NOLINT
    std::string first_name;                       // NOLINT
    std::string last_name;                        // NOLINT
    std::optional<std::int64_t> organization_id;  // NOLINT
    std::string password_hash;                    // NOLINT

    auto introspect() {
        return std::tie(
            id, email, first_name, last_name, organization_id, password_hash
        );
    }
};

}  // namespace pawspective::models
