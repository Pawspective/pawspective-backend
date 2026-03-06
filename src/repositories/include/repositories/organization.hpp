#pragma once

#include <optional>
#include <userver/storages/postgres/cluster.hpp>
#include <vector>
#include "models/organization.hpp"

namespace pawspective::repositories {

class OrganizationRepository {
public:
    explicit OrganizationRepository(userver::storages::postgres::ClusterPtr pg_cluster);

    [[nodiscard]] std::optional<models::Organization> GetById(int64_t organization_id) const;
    models::Organization Create(const models::Organization& organization) const;
    models::Organization Update(const models::OrganizationUpdate& organization) const;
    [[nodiscard]] std::vector<models::Organization> FindByNameContaining(std::string_view name) const;

private:
    const userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace pawspective::repositories
