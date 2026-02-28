#include "services/user_service.hpp"

#include <optional>
#include <userver/storages/postgres/exceptions.hpp>
#include "services/exception.hpp"
#include "user.hpp"
#include "utils/hasher.hpp"

namespace pawspective::services {
UserService::UserService(const repositories::UserRepository& user_repository) : user_repository_(user_repository) {}

models::User UserService::GetUserById(std::int64_t user_id) const {
    auto user = user_repository_.get_by_id(user_id);
    if (!user) {
        throw UserNotFoundException();
    }
    return *user;
}

models::User UserService::RegisterUser(const dto::UserRegisterDTO& dto) const {
    if (user_repository_.get_by_email(dto.email)) {
        throw UserAlreadyExistsException();
    }
    models::User user = models::User::from_register_dto(dto);
    user.password_hash = utils::crypto::GenerateHash(dto.password);
    try {
        return user_repository_.create_user(user);
    } catch (const userver::storages::postgres::UniqueViolation&) {
        throw UserAlreadyExistsException();
    }
}

models::User UserService::AuthenticateUser(const std::string& email, const std::string& password) const {
    auto user = user_repository_.get_by_email(email);
    if (!user) {
        throw UserNotFoundException();
    }
    if (!utils::crypto::VerifyHash(password, user->password_hash)) {
        throw UserNotFoundException();
    }
    return *user;
}

std::optional<std::int64_t> UserService::GetOrganizationId(std::int64_t user_id) const {
    return user_repository_.get_organization_by_user_id(user_id);
}

models::User UserService::UpdateUser(std::int64_t user_id, const dto::UserUpdateDTO& dto) const {
    models::User new_data = models::User::from_update_dto(dto);
    if (auto opt = user_repository_.get_by_id(user_id); !opt.has_value()) {
        throw UserNotFoundException();
    }
    try {
        return user_repository_.update_user(user_id, new_data);
    } catch (const userver::storages::postgres::UniqueViolation&) {
        throw UserAlreadyExistsException();
    }
}

bool UserService::CanUserCreateOrganization(std::int64_t user_id) const {
    return user_repository_.check_can_create_shelter(user_id);
}
}  // namespace pawspective::services
