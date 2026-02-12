#pragma once

#include <cstdint>
#include <optional>
#include <string>
<<<<<<< HEAD
#include <userver/storages/postgres/cluster.hpp>
=======
>>>>>>> df60ae5 (refactor(user): change user_repository.hpp)
#include "../../models/include/user.hpp"

namespace pawspective::repositories {

class UserRepository {
private:
<<<<<<< HEAD
    userver::storages::postgres::ClusterPtr pg_cluster_;
=======
    userver::storages::postgres::ClusterPtr pg_cluster_{};
>>>>>>> df60ae5 (refactor(user): change user_repository.hpp)

public:
    explicit UserRepository(userver::storages::postgres::ClusterPtr pg_cluster);

<<<<<<< HEAD
    [[nodiscard]] std::optional<models::User> get_by_id(std::int64_t id) const;
    [[nodiscard]] std::optional<models::User> get_by_email(
        const std::string &email
    ) const;
    // NOLINTBEGIN(modernize-use-nodiscard)
    models::User create_user(const models::User &user) const;
    models::User update_user(std::int64_t id, const models::User &user_upd)
        const;
    models::User link_organization(std::int64_t id, std::int64_t org_id) const;
    models::User
    set_password_hash(std::int64_t id, const std::string &password_hash) const;
    bool delete_user(std::int64_t id) const;
    // NOLINTEND(modernize-use-nodiscard)
=======
    std::optional<models::User> get_by_id(std::int64_t id);
    std::optional<models::User> get_by_email(const std::string &email);
    models::User create_user(const models::User &user);
    models::User update_user(std::int64_t id, const models::User &user_upd);
    models::User link_organization(std::int64_t id, std::int64_t org_id);
    models::User
    set_password_hash(std::int64_t id, const std::string &password_hash);
    bool delete_user(std::int64_t id);
>>>>>>> df60ae5 (refactor(user): change user_repository.hpp)
    [[nodiscard]] bool check_can_create_shelter(std::int64_t id) const;
    // std::optional<models::Organization>
    // get_organization_by_user_id(std::int64_t id); // TODO: Uncomment after
    // creation Organization
};
}  // namespace pawspective::repositories