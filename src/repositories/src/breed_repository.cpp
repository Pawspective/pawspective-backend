#include "breed_repository.hpp"

namespace pawspective::repositories {

BreedRepository::BreedRepository(userver::storages::postgres::ClusterPtr pg_cluster)
    : pg_cluster_(std::move(pg_cluster)) {}

std::optional<models::Breed> BreedRepository::GetById(int64_t id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, animal_type, name FROM breeds WHERE id = $1",
        id
    );
    if (result.IsEmpty()) {
        return std::nullopt;
    }
    return result.AsSingleRow<models::Breed>(userver::storages::postgres::kRowTag);
}
std::vector<models::Breed> BreedRepository::GetByType(models::AnimalType type) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, animal_type, name FROM breeds WHERE animal_type = $1",
        type
    );
    return result.AsContainer<std::vector<models::Breed>>(userver::storages::postgres::kRowTag);
}

std::vector<models::Breed> BreedRepository::GetByIds(const std::vector<int64_t>& ids) const {
    if (ids.empty()) {
        return {};
    }
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, animal_type, name FROM breeds WHERE id = ANY($1)",
        ids
    );
    return result.AsContainer<std::vector<models::Breed>>(userver::storages::postgres::kRowTag);
}

}  // namespace pawspective::repositories
