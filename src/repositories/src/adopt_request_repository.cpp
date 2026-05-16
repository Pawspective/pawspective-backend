#include "../include/adopt_request_repository.hpp"

namespace pawspective::repositories {

AdoptRequestRepository::AdoptRequestRepository(userver::storages::postgres::ClusterPtr pg_cluster)
    : pg_cluster_(std::move(pg_cluster)) {}

models::AdoptRequest AdoptRequestRepository::Create(std::int64_t animal_id, std::int64_t user_id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO adopt_requests (animal_id, user_id) VALUES ($1, $2) "
        "RETURNING id, animal_id, user_id",
        animal_id,
        user_id
    );
    return result.AsSingleRow<models::AdoptRequest>(userver::storages::postgres::kRowTag);
}

std::optional<models::AdoptRequest> AdoptRequestRepository::GetById(std::int64_t id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, animal_id, user_id FROM adopt_requests WHERE id = $1",
        id
    );
    if (result.IsEmpty()) {
        return std::nullopt;
    }
    return result.AsSingleRow<models::AdoptRequest>(userver::storages::postgres::kRowTag);
}

}  // namespace pawspective::repositories
