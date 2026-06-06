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
        "SELECT id, name, description, avatar_url, city_id FROM organizations WHERE id = $1",
        organization_id
    );
    if (result.IsEmpty()) {
        return std::nullopt;
    }
    return result.AsSingleRow<models::Organization>(userver::storages::postgres::kRowTag);
}

std::pair<std::vector<models::Organization>, std::int64_t> OrganizationRepository::FindByNameContainingPaginated(
    std::string_view name,
    int page,
    int limit
) const {
    if (page < 1 || limit <= 0) {
        throw std::invalid_argument("page must be >= 1 and limit must be > 0");
    }

    static const pawspective::utils::sql::QueryWhitelist kWhitelist{
        .filter_fields = {{"name", "name"}},
        .sort_fields = {},
    };

    pawspective::utils::sql::QueryFilter count_filter;
    count_filter.conditions.emplace_back(pawspective::utils::sql::Condition::Ilike("name", std::string(name)));
    const auto count_clause = pawspective::utils::sql::BuildQueryClause(count_filter, kWhitelist);
    auto count_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT COUNT(*) FROM organizations" + count_clause.query,
        count_clause.parameters
    );
    const std::int64_t total_count = count_result.AsSingleRow<std::int64_t>();

    pawspective::utils::sql::QueryFilter data_filter;
    data_filter.conditions.emplace_back(pawspective::utils::sql::Condition::Ilike("name", std::string(name)));
    data_filter.page_spec.limit = limit;
    data_filter.page_spec.offset = static_cast<int>(static_cast<std::int64_t>(page - 1) * limit);
    const auto data_clause = pawspective::utils::sql::BuildQueryClause(data_filter, kWhitelist);
    auto data_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, name, description, avatar_url, city_id FROM organizations" + data_clause.query,
        data_clause.parameters
    );
    auto orgs = data_result.AsContainer<std::vector<models::Organization>>(userver::storages::postgres::kRowTag);

    return {std::move(orgs), total_count};
}

models::Organization OrganizationRepository::Create(const models::Organization& organization) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO organizations (name, description, avatar_url, city_id) VALUES ($1, $2, $3, $4) RETURNING id, "
        "name, description, avatar_url, city_id",
        organization.name,
        organization.description,
        organization.avatar_url,
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
    if (org.avatar_url.has_value()) {
        updates.push_back(fmt::format("avatar_url = ${}", parameters.Size() + 1));
        parameters.PushBack(org.avatar_url->empty() ? std::nullopt : org.avatar_url);
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
        "RETURNING id, name, description, avatar_url, city_id",
        fmt::join(updates, ", ")
    );
    auto result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, query, parameters);
    if (result.IsEmpty()) {
        return std::nullopt;
    }
    return result.AsSingleRow<models::Organization>(userver::storages::postgres::kRowTag);
}
bool OrganizationRepository::Delete(int64_t organization_id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "DELETE FROM organizations WHERE id = $1",
        organization_id
    );
    return result.RowsAffected() > 0;
}

}  // namespace pawspective::repositories
