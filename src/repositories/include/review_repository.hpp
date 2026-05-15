#pragma once

#include <userver/storages/postgres/cluster.hpp>
#include "review.hpp"

namespace pawspective::repositories {

class ReviewRepository {
public:
    explicit ReviewRepository(userver::storages::postgres::ClusterPtr pg_cluster);

    [[nodiscard]] std::optional<models::Review> GetById(int64_t review_id) const;
    [[nodiscard]] std::pair<std::vector<models::Review>, int64_t> GetByOrganizationIdPaginated(
        int64_t organization_id,
        int page,
        int limit
    ) const;
    models::Review Create(const models::Review& review) const;
    models::Review UpdateTextById(int64_t review_id, const std::string& text) const;
    bool Delete(int64_t review_id) const;

private:
    const userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace pawspective::repositories
