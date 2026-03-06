#include "../include/user_repository.hpp"
#include <boost/algorithm/string/join.hpp>
#include <optional>
#include <userver/storages/postgres/cluster.hpp>

namespace pawspective::repositories {

UserRepository::UserRepository(userver::storages::postgres::ClusterPtr pg_cluster)
    : pg_cluster_(std::move(pg_cluster)) {}

[[nodiscard]] std::optional<models::User> UserRepository::get_by_id(const std::int64_t id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, email, first_name, last_name, organization_id, password_hash "
        "FROM users WHERE id = $1",
        id
    );
    if (result.IsEmpty()) {
        return std::nullopt;
    }
    return result.AsSingleRow<models::User>(userver::storages::postgres::kRowTag);
}

[[nodiscard]] std::optional<models::User> UserRepository::get_by_email(const std::string& email) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, email, first_name, last_name, organization_id, password_hash "
        "FROM users WHERE email = $1",
        email
    );
    if (result.IsEmpty()) {
        return std::nullopt;
    }
    return result.AsSingleRow<models::User>(userver::storages::postgres::kRowTag);
}

models::User UserRepository::create_user(const models::User& user) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO users (email, first_name, last_name, organization_id, password_hash) "
        "VALUES ($1, $2, $3, $4, $5) "
        "RETURNING id, email, first_name, last_name, organization_id, password_hash",
        user.email,
        user.first_name,
        user.last_name,
        user.organization_id,
        user.password_hash
    );
    return result.AsSingleRow<models::User>(userver::storages::postgres::kRowTag);
}

models::User UserRepository::update_user(std::int64_t id, const models::User& user_upd) const {
    userver::storages::postgres::ParameterStore parameters;
    std::vector<std::string> updates;
    parameters.PushBack(id);
    int param_index = 2;
    auto [upd_id, upd_email, upd_first_name, upd_last_name, upd_org_id, upd_password_hash] = user_upd.introspect();
    if (!upd_email.empty()) {
        updates.push_back("email = $" + std::to_string(param_index++));
        parameters.PushBack(upd_email);
    }
    if (!upd_first_name.empty()) {
        updates.push_back("first_name = $" + std::to_string(param_index++));
        parameters.PushBack(upd_first_name);
    }
    if (!upd_last_name.empty()) {
        updates.push_back("last_name = $" + std::to_string(param_index++));
        parameters.PushBack(upd_last_name);
    }
    if (!upd_password_hash.empty()) {
        updates.push_back("password_hash = $" + std::to_string(param_index++));
        parameters.PushBack(upd_password_hash);
    }
    if (updates.empty()) {
        return *get_by_id(id);
    }
    std::string query =
        "UPDATE users SET " + boost::algorithm::join(updates, ", ") +
        " WHERE id = $1 "
        "RETURNING id, email, first_name, last_name, organization_id, password_hash";
    auto result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, query, parameters);
    return result.AsSingleRow<models::User>(userver::storages::postgres::kRowTag);
}

models::User UserRepository::link_organization(std::int64_t id, std::int64_t org_id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "UPDATE users SET organization_id = $2 "
        "WHERE id = $1 "
        "RETURNING id, email, first_name, last_name, organization_id, password_hash",
        id,
        org_id
    );
    return result.AsSingleRow<models::User>(userver::storages::postgres::kRowTag);
}

[[nodiscard]] bool UserRepository::check_can_create_shelter(std::int64_t id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT organization_id FROM users WHERE id = $1",
        id
    );
    if (result.IsEmpty()) {
        return false;
    }
    return !result[0][0].IsNull();
}

[[nodiscard]] std::optional<std::int64_t> UserRepository::get_organization_id_by_user_id(std::int64_t id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT organization_id FROM users WHERE id = $1",
        id
    );

    if (result.IsEmpty() || result[0][0].IsNull()) {
        return std::nullopt;
    }

    return result.AsSingleRow<std::int64_t>();
}

[[nodiscard]] std::optional<models::Organization> UserRepository::get_organization_by_user_id(std::int64_t id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT o.id, o.name, o.description, o.city_id "
        "FROM users u "
        "JOIN organizations o ON u.organization_id = o.id "
        "WHERE u.id = $1",
        id
    );

    if (result.IsEmpty()) {
        return std::nullopt;
    }

    return result.AsSingleRow<models::Organization>(userver::storages::postgres::kRowTag);
}
}  // namespace pawspective::repositories
