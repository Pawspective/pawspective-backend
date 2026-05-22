#pragma once

#include <optional>
#include <userver/storages/postgres/cluster.hpp>
#include <vector>

#include "../../models/include/adopt_request.hpp"

namespace pawspective::repositories {

class AdoptRequestRepository {
public:
    explicit AdoptRequestRepository(userver::storages::postgres::ClusterPtr pg_cluster);

    models::AdoptRequest Create(std::int64_t animal_id, std::int64_t user_id) const;
    [[nodiscard]] std::optional<models::AdoptRequest> GetById(std::int64_t id) const;
    [[nodiscard]] std::pair<std::vector<models::AdoptRequest>, std::int64_t> GetByOrganizationIdPaginated(
        std::int64_t organization_id,
        int page,
        int limit
    ) const;
    [[nodiscard]] std::unordered_set<int64_t> GetAnimalIdsByUserId(std::int64_t user_id) const;
    bool DeleteById(std::int64_t id) const;
    bool DeleteByAnimalId(std::int64_t animal_id) const;

private:
    const userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace pawspective::repositories
