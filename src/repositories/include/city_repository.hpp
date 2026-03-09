#pragma once

#include <optional>
#include <userver/storages/postgres/cluster.hpp>
#include <vector>
#include "city.hpp"

namespace pawspective::repositories {

class CityRepository {
public:
    explicit CityRepository(userver::storages::postgres::ClusterPtr pg_cluster);

    [[nodiscard]] std::optional<models::City> GetById(int64_t id) const;
    [[nodiscard]] std::vector<models::City> GetAll() const;

private:
    const userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace pawspective::repositories
