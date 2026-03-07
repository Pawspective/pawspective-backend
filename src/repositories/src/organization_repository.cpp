#include "organization_repository.hpp"

#include <fmt/format.h>
#include <boost/algorithm/string/join.hpp>

namespace pawspective::repositories {

namespace {
std::string EscapeForLike(std::string_view input) {
    std::string escaped;
    escaped.reserve(input.size());

    for (char c : input) {
        if (c == '%' || c == '_' || c == '\\') {
            escaped += '\\';
        }
        escaped += c;
    }
    return escaped;
}
}  // namespace

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
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, name, description, city_id FROM organizations WHERE name ILIKE '%' || $1 || '%' ESCAPE '\\' LIMIT "
        "50",
        EscapeForLike(name)
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

models::Organization OrganizationRepository::Update(const models::Organization& org) const {
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
    auto query = fmt::format(
        "UPDATE organizations SET {} WHERE id = $1 "
        "RETURNING id, name, description, city_id",
        fmt::join(updates, ", ")
    );
    auto result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, query, parameters);
    return result.AsSingleRow<models::Organization>(userver::storages::postgres::kRowTag);
}

}  // namespace pawspective::repositories
