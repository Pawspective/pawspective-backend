#pragma once

#include <cstdint>
#include <optional>
#include <userver/storages/postgres/cluster.hpp>
#include <vector>
#include "../../models/include/post.hpp"

namespace pawspective::repositories {

class PostRepository {
public:
    explicit PostRepository(userver::storages::postgres::ClusterPtr pg_cluster);

    [[nodiscard]] std::optional<models::Post> GetById(int64_t post_id) const;
    [[nodiscard]] std::pair<std::vector<models::Post>, int64_t> GetByOrganizationIdPaginated(
        int64_t organization_id,
        int page,
        int limit
    ) const;
    models::Post Create(const models::Post& post) const;
    std::optional<models::Post> Update(const models::Post& post) const;
    bool Delete(int64_t post_id) const;

private:
    const userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace pawspective::repositories
