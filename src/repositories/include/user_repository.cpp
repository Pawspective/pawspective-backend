#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include "../../models/include/user.hpp"

namespace pawspective::repositories {

class UserRepository {
private:
    userver::storages::postgres::ClusterPtr pg_cluster_{};

public:
    UserRepository(userver::storages::postgres::ClusterPtr pg_cluster)
        : pg_cluster_(std::move(pg_cluster)) {
    }

    std::optional<models::User> get_by_id(std::int64_t id);
    std::optional<models::User> get_by_email(const std::string &email);
    models::User create_user(const models::User &user);
    models::User update_user(std::int64_t id, const models::User &user_upd);
    models::User link_organization(std::int64_t id, std::int64_t org_id);
    models::User
    set_password_hash(std::int64_t id, const std::string &password_hash);
    bool delete_user(std::int64_t id);
    bool check_can_create_shelter(std::int64_t id);
    // std::optional<models::Organization>
    // get_organization_by_user_id(std::int64_t id); // TODO: Uncomment after
    // creation Organization
};
}  // namespace pawspective::repositories