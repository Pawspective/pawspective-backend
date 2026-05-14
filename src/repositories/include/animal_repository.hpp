#pragma once

#include <optional>
#include <userver/storages/postgres/cluster.hpp>
#include <vector>

#include "../../models/include/animal.hpp"
#include "../../models/include/animal_filters.hpp"

namespace pawspective::repositories {

class AnimalRepository {
public:
    explicit AnimalRepository(userver::storages::postgres::ClusterPtr pg_cluster);

    [[nodiscard]] std::optional<models::Animal> GetById(std::int64_t id) const;
    [[nodiscard]] std::pair<std::vector<models::Animal>, std::int64_t> GetByOrganizationIdPaginated(
        std::int64_t org_id,
        int page,
        int limit
    ) const;
    models::Animal Create(const models::Animal& animal) const;
    std::optional<models::Animal> Update(const models::Animal& animal) const;
    [[nodiscard]] models::AnimalFilters GetAvailableFilters() const;
    [[nodiscard]] std::pair<std::vector<models::Animal>, std::int64_t> FindByFiltersPaginated(
        const models::AnimalFilters& filter,
        int page,
        int limit
    ) const;
    void Delete(std::int64_t id) const;

private:
    const userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace pawspective::repositories