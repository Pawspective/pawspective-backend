#pragma once

#include <optional>
#include <userver/storages/postgres/cluster.hpp>
#include <vector>
#include "../../models/include/breed.hpp"

namespace pawspective::repositories {

class BreedRepository {
public:
    explicit BreedRepository(userver::storages::postgres::ClusterPtr pg_cluster);

    [[nodiscard]] std::optional<models::Breed> GetById(int64_t id) const;
    [[nodiscard]] std::vector<models::Breed> GetByType(models::AnimalType type) const;
    [[nodiscard]] std::vector<models::Breed> GetByIds(const std::vector<int64_t>& ids) const;

private:
    const userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace pawspective::repositories
