#include <review_repository.hpp>

namespace pawspective::repositories {
ReviewRepository::ReviewRepository(userver::storages::postgres::ClusterPtr pg_cluster)
    : pg_cluster_(std::move(pg_cluster)) {}

[[nodiscard]] std::optional<models::Review> ReviewRepository::GetById(int64_t review_id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, animal_id, user_id, text, created_at FROM reviews WHERE id = $1",
        review_id
    );
    if (result.IsEmpty()) {
        return std::nullopt;
    }
    return result.AsSingleRow<models::Review>(userver::storages::postgres::kRowTag);
}

[[nodiscard]] std::pair<std::vector<models::Review>, int64_t> ReviewRepository::GetByOrganizationIdPaginated(
    int64_t organization_id,
    int page,
    int limit
) const {
    if (page < 1 || limit <= 0) {
        throw std::invalid_argument("page must be >= 1 and limit must be > 0");
    }

    auto count_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT COUNT(*) FROM reviews r JOIN animals a ON r.animal_id = a.id WHERE a.organization_id = $1",
        organization_id
    );
    const std::int64_t total_count = count_result.AsSingleRow<std::int64_t>();
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT r.id, r.animal_id, r.user_id, r.text, r.created_at "
        "FROM reviews r "
        "JOIN animals a ON r.animal_id = a.id "
        "WHERE a.organization_id = $1 "
        "ORDER BY r.created_at DESC "
        "LIMIT $2 OFFSET $3",
        organization_id,
        limit,
        static_cast<int>(static_cast<std::int64_t>(page - 1) * limit)
    );
    auto reviews = result.AsContainer<std::vector<models::Review>>(userver::storages::postgres::kRowTag);

    return {std::move(reviews), total_count};
}

bool ReviewRepository::ExistsUserReviewForAnimal(int64_t user_id, int64_t animal_id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT EXISTS(SELECT 1 FROM reviews WHERE user_id = $1 AND animal_id = $2)",
        user_id,
        animal_id
    );
    return result.AsSingleRow<bool>();
}

models::Review ReviewRepository::Create(const models::Review& review) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO reviews (animal_id, user_id, text) VALUES ($1, $2, $3) RETURNING id, animal_id, user_id, text, "
        "created_at",
        review.animal_id,
        review.user_id,
        review.text
    );
    return result.AsSingleRow<models::Review>(userver::storages::postgres::kRowTag);
}

std::optional<models::Review> ReviewRepository::UpdateTextById(int64_t review_id, const std::string& text) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "UPDATE reviews SET text = $1 WHERE id = $2 RETURNING id, animal_id, user_id, text, created_at",
        text,
        review_id
    );
    if (result.IsEmpty()) {
        return std::nullopt;
    }
    return result.AsSingleRow<models::Review>(userver::storages::postgres::kRowTag);
}

bool ReviewRepository::Delete(int64_t review_id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "DELETE FROM reviews WHERE id = $1",
        review_id
    );
    return result.RowsAffected() > 0;
}

}  // namespace pawspective::repositories
