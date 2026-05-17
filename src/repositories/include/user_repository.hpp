#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <userver/storages/postgres/cluster.hpp>
#include "organization.hpp"
#include "user.hpp"

namespace pawspective::repositories {

class UserRepository {
    userver::storages::postgres::ClusterPtr pg_cluster_;

public:
    explicit UserRepository(userver::storages::postgres::ClusterPtr pg_cluster);

    [[nodiscard]] std::optional<models::User> get_by_id(std::int64_t id) const;
    [[nodiscard]] std::optional<models::User> get_by_email(const std::string& email) const;
    [[nodiscard]] std::unordered_map<std::int64_t, models::User> get_by_ids(const std::vector<std::int64_t>& ids) const;
    // NOLINTBEGIN(modernize-use-nodiscard)
    models::User create_user(const models::User& user) const;
    models::User update_user(std::int64_t id, const models::User& user_upd) const;
    models::User link_organization(std::int64_t id, std::int64_t org_id) const;
    // NOLINTEND(modernize-use-nodiscard)
    [[nodiscard]] bool check_can_create_shelter(std::int64_t id) const;
    [[nodiscard]] std::optional<std::int64_t> get_organization_id_by_user_id(std::int64_t id) const;
    [[nodiscard]] std::optional<models::Organization> get_organization_by_user_id(std::int64_t id) const;
    bool delete_user(std::int64_t id) const;
};
}  // namespace pawspective::repositories