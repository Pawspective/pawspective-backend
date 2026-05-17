#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>
#include "user.hpp"
#include "user_register_dto.hpp"
#include "user_repository.hpp"
#include "user_update_dto.hpp"

namespace pawspective::services {

class UserService {
public:
    explicit UserService(const repositories::UserRepository& user_repository);

    models::User GetUserById(std::int64_t user_id) const;
    std::vector<models::User> GetUsersByIds(const std::vector<std::int64_t>& ids) const;
    models::User RegisterUser(const dto::UserRegisterDTO& dto) const;
    models::User AuthenticateUser(const std::string& email, const std::string& password) const;
    models::User LinkOrganization(std::int64_t user_id, std::int64_t organization_id) const;
    std::optional<std::int64_t> GetOrganizationId(std::int64_t user_id) const;
    models::User UpdateUser(std::int64_t user_id, const dto::UserUpdateDTO& dto) const;
    bool CanUserCreateOrganization(std::int64_t user_id) const;
    void DeleteUser(std::int64_t user_id) const;

private:
    const repositories::UserRepository& user_repository_;
};
}  // namespace pawspective::services
