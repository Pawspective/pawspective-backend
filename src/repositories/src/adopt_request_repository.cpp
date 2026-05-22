#include "../include/adopt_request_repository.hpp"

#include <stdexcept>

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

std::unordered_set<int64_t> AdoptRequestRepository::GetAnimalIdsByUserId(std::int64_t user_id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT animal_id FROM adopt_requests WHERE user_id = $1",
        user_id
    );
    std::unordered_set<int64_t> animal_ids;
    for (const auto& row : result) {
        animal_ids.insert(row["animal_id"].As<int64_t>());
    }
    return animal_ids;
}

std::pair<std::vector<models::AdoptRequest>, std::int64_t> AdoptRequestRepository::GetByOrganizationIdPaginated(
    std::int64_t organization_id,
    int page,
    int limit
) const {
    if (page < 1 || limit <= 0) {
        throw std::invalid_argument("page must be >= 1 and limit must be > 0");
    }

    auto count_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT COUNT(*) FROM adopt_requests ar "
        "JOIN animals a ON ar.animal_id = a.id "
        "WHERE a.organization_id = $1",
        organization_id
    );
    const std::int64_t total_count = count_result.AsSingleRow<std::int64_t>();

    auto data_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT ar.id, ar.animal_id, ar.user_id "
        "FROM adopt_requests ar "
        "JOIN animals a ON ar.animal_id = a.id "
        "WHERE a.organization_id = $1 "
        "ORDER BY ar.id ASC "
        "LIMIT $2 OFFSET $3",
        organization_id,
        limit,
        static_cast<std::int64_t>(page - 1) * limit
    );
    auto requests = data_result.AsContainer<std::vector<models::AdoptRequest>>(userver::storages::postgres::kRowTag);

    return {std::move(requests), total_count};
}

bool AdoptRequestRepository::DeleteById(std::int64_t id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "DELETE FROM adopt_requests WHERE id = $1",
        id
    );
    return result.RowsAffected() > 0;
}

bool AdoptRequestRepository::DeleteByAnimalId(std::int64_t animal_id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "DELETE FROM adopt_requests WHERE animal_id = $1",
        animal_id
    );
    return result.RowsAffected() > 0;
}

}  // namespace pawspective::repositories
