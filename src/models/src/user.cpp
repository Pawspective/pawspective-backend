#include "../include/user.hpp"
#include <optional>
#include "../../dto/include/user_dto.hpp"
#include "../../dto/include/user_register_dto.hpp"
#include "../../dto/include/user_update_dto.hpp"

namespace pawspective::models {

User User::from_register_dto(const dto::UserRegisterDTO &reg) {
    User user;

    user.email = reg.email;
    user.first_name = reg.first_name;
    user.last_name = reg.last_name;
    user.password_hash = reg.password;
    user.id = 0;
    user.organization_id = std::nullopt;

    return user;
}

User User::from_update_dto(const dto::UserUpdateDTO &upd) {
    User user;

    if (upd.email.has_value()) {
        user.email = *upd.email;
    }
    if (upd.password.has_value()) {
        user.password_hash = *upd.password;
    }
    if (upd.first_name.has_value()) {
        user.first_name = *upd.first_name;
    }
    if (upd.last_name.has_value()) {
        user.last_name = *upd.last_name;
    }

    return user;
}

dto::UserDTO User::to_dto(const User &model) {
    dto::UserDTO dto;

    dto.id = model.id;
    dto.email = model.email;
    dto.first_name = model.first_name;
    dto.last_name = model.last_name;
    dto.organization_id = model.organization_id;

    return dto;
}

}  // namespace pawspective::models