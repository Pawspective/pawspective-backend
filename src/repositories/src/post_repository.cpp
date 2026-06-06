#include "../include/post_repository.hpp"

#include <fmt/format.h>
#include <boost/algorithm/string/join.hpp>

namespace pawspective::repositories {

PostRepository::PostRepository(userver::storages::postgres::ClusterPtr pg_cluster)
    : pg_cluster_(std::move(pg_cluster)) {}

[[nodiscard]] std::optional<models::Post> PostRepository::GetById(int64_t post_id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, organization_id, text, photos, created_at FROM posts WHERE id = $1",
        post_id
    );
    if (result.IsEmpty()) {
        return std::nullopt;
    }
    return result.AsSingleRow<models::Post>(userver::storages::postgres::kRowTag);
}

std::pair<std::vector<models::Post>, std::int64_t> PostRepository::GetByOrganizationIdPaginated(
    int64_t organization_id,
    int page,
    int limit
) const {
    if (page < 1 || limit <= 0) {
        throw std::invalid_argument("page must be >= 1 and limit must be > 0");
    }

    auto count_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT COUNT(*) FROM posts WHERE organization_id = $1",
        organization_id
    );
    const std::int64_t total_count = count_result.AsSingleRow<std::int64_t>();

    auto data_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, organization_id, text, photos, created_at "
        "FROM posts WHERE organization_id = $1 "
        "ORDER BY created_at DESC "
        "LIMIT $2 OFFSET $3",
        organization_id,
        limit,
        static_cast<std::int64_t>(page - 1) * limit
    );
    auto posts = data_result.AsContainer<std::vector<models::Post>>(userver::storages::postgres::kRowTag);

    return {std::move(posts), total_count};
}

models::Post PostRepository::Create(const models::Post& post) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO posts (organization_id, text, photos) VALUES ($1, $2, $3) RETURNING id, organization_id, text, "
        "photos, created_at",
        post.organization_id,
        post.text,
        post.photos
    );
    return result.AsSingleRow<models::Post>(userver::storages::postgres::kRowTag);
}

std::optional<models::Post> PostRepository::Update(
    const models::Post& post,
    const std::optional<std::vector<std::string>>& upd_photos
) const {
    userver::storages::postgres::ParameterStore parameters;
    std::vector<std::string> updates;
    parameters.PushBack(post.id);

    auto add_field = [&](const std::string& field_name, auto value) {
        updates.push_back(fmt::format("{} = ${}", field_name, parameters.Size() + 1));
        parameters.PushBack(value);
    };
    if (!post.text.empty()) {
        add_field("text", post.text);
    }
    if (upd_photos.has_value()) {
        add_field("photos", upd_photos.value());
    }
    if (updates.empty()) {
        return GetById(post.id);
    }

    auto query = fmt::format(
        "UPDATE posts SET {} WHERE id = $1 "
        "RETURNING id, organization_id, text, photos, created_at",
        fmt::join(updates, ", ")
    );

    auto result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, query, parameters);

    if (result.IsEmpty()) {
        return std::nullopt;
    }
    return result.AsSingleRow<models::Post>(userver::storages::postgres::kRowTag);
}

bool PostRepository::Delete(int64_t post_id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "DELETE FROM posts WHERE id = $1",
        post_id
    );
    return result.RowsAffected() > 0;
}

}  // namespace pawspective::repositories
