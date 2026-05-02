#pragma once

#include <optional>
#include <userver/storages/postgres/cluster.hpp>
#include <vector>
#include "organization.hpp"

namespace pawspective::repositories {

class OrganizationRepository {
public:
    explicit OrganizationRepository(userver::storages::postgres::ClusterPtr pg_cluster);

    [[nodiscard]] std::optional<models::Organization> GetById(int64_t organization_id) const;
    models::Organization Create(const models::Organization& organization) const;
    std::optional<models::Organization> Update(const models::Organization& organization) const;
    [[nodiscard]] std::pair<std::vector<models::Organization>, std::int64_t> FindByNameContainingPaginated(
        std::string_view name,
        int page,
        int limit
    ) const;

private:
    const userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace pawspective::repositories
