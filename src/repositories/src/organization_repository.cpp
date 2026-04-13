#include "../include/organization_repository.hpp"

#include <fmt/format.h>
#include <boost/algorithm/string/join.hpp>

#include <utils/sql_builder.hpp>

namespace pawspective::repositories {

OrganizationRepository::OrganizationRepository(userver::storages::postgres::ClusterPtr pg_cluster)
    : pg_cluster_(std::move(pg_cluster)) {}

[[nodiscard]] std::optional<models::Organization> OrganizationRepository::GetById(int64_t organization_id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, name, description, city_id FROM organizations WHERE id = $1",
        organization_id
    );
    if (result.IsEmpty()) {
        return std::nullopt;
    }
    return result.AsSingleRow<models::Organization>(userver::storages::postgres::kRowTag);
}

[[nodiscard]] std::vector<models::Organization> OrganizationRepository::FindByNameContaining(const std::string_view name
) const {
    pawspective::utils::sql::QueryFilter filter;
    filter.conditions.emplace_back(pawspective::utils::sql::Condition::Ilike("name", std::string(name)));
    filter.page_spec.limit = 50;
    static const pawspective::utils::sql::QueryWhitelist kWhitelist{
        .filter_fields = {{"name", "name"}},
        .sort_fields = {},
    };
    const auto query_clause = pawspective::utils::sql::BuildQueryClause(filter, kWhitelist);

    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, name, description, city_id FROM organizations" + query_clause.query,
        query_clause.parameters
    );
    return result.AsContainer<std::vector<models::Organization>>(userver::storages::postgres::kRowTag);
}

models::Organization OrganizationRepository::Create(const models::Organization& organization) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO organizations (name, description, city_id) VALUES ($1, $2, $3) RETURNING id, name, description, "
        "city_id",
        organization.name,
        organization.description,
        organization.city_id
    );
    return result.AsSingleRow<models::Organization>(userver::storages::postgres::kRowTag);
}

std::optional<models::Organization> OrganizationRepository::Update(const models::Organization& org) const {
    userver::storages::postgres::ParameterStore parameters;
    std::vector<std::string> updates;
    parameters.PushBack(org.id);
    if (!org.name.empty()) {
        updates.push_back(fmt::format("name = ${}", parameters.Size() + 1));
        parameters.PushBack(org.name);
    }
    if (org.description.has_value()) {
        updates.push_back(fmt::format("description = ${}", parameters.Size() + 1));
        parameters.PushBack(org.description->empty() ? std::nullopt : org.description);
    }
    if (org.city_id != -1) {
        updates.push_back(fmt::format("city_id = ${}", parameters.Size() + 1));
        parameters.PushBack(org.city_id);
    }
    if (updates.empty()) {
        return GetById(org.id);
    }
    auto query = fmt::format(
        "UPDATE organizations SET {} WHERE id = $1 "
        "RETURNING id, name, description, city_id",
        fmt::join(updates, ", ")
    );
    auto result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, query, parameters);
    if (result.IsEmpty()) {
        return std::nullopt;
    }
    return result.AsSingleRow<models::Organization>(userver::storages::postgres::kRowTag);
}

}  // namespace pawspective::repositories
