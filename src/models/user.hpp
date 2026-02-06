#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include "dto/user_dto.hpp"
#include "dto/user_register_dto.hpp"
#include "dto/user_update_dto.hpp"

namespace pawspective::models {

struct User {
    static User parse(const dto::UserRegisterDTO &reg);
    static User parse(const dto::UserUpdateDTO &upd);
    static dto::UserDTO parse(const User &model);
    std::int64_t id;
    std::string email;
    std::string first_name;
    std::string last_name;
    std::optional<std::int64_t> organization_id;
    std::string password_hash;

    // maybe add Introspect
};

}  // namespace pawspective::models
