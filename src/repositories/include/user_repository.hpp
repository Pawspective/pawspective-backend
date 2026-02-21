#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <userver/storages/postgres/cluster.hpp>
#include "../../models/include/user.hpp"

namespace pawspective::repositories {

class UserRepository {
    userver::storages::postgres::ClusterPtr pg_cluster_;

public:
    explicit UserRepository(userver::storages::postgres::ClusterPtr pg_cluster);

    [[nodiscard]] std::optional<models::User> get_by_id(std::int64_t id) const;
    [[nodiscard]] std::optional<models::User> get_by_email(const std::string& email) const;
    // NOLINTBEGIN(modernize-use-nodiscard)
    models::User create_user(const models::User& user) const;
    models::User update_user(std::int64_t id, const models::User& user_upd) const;
    models::User link_organization(std::int64_t id, std::int64_t org_id) const;
    models::User set_password_hash(std::int64_t id, const std::string& password_hash) const;
    // NOLINTEND(modernize-use-nodiscard)
    [[nodiscard]] bool check_can_create_shelter(std::int64_t id) const;
    [[nodiscard]] std::optional<std::int64_t> get_organization_by_user_id(std::int64_t id) const;
};
}  // namespace pawspective::repositories