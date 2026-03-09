#include "city_repository.hpp"

namespace pawspective::repositories {

CityRepository::CityRepository(userver::storages::postgres::ClusterPtr pg_cluster)
    : pg_cluster_(std::move(pg_cluster)) {}

std::optional<models::City> CityRepository::GetById(int64_t id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, name FROM cities WHERE id = $1",
        id
    );
    if (result.IsEmpty()) {
        return std::nullopt;
    }
    return result.AsSingleRow<models::City>(userver::storages::postgres::kRowTag);
}

std::vector<models::City> CityRepository::GetAll() const {
    auto result =
        pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave, "SELECT id, name FROM cities");
    return result.AsContainer<std::vector<models::City>>(userver::storages::postgres::kRowTag);
}

}  // namespace pawspective::repositories
