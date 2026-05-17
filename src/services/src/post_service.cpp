#include "post_service.hpp"

#include "services/exception.hpp"

namespace pawspective::services {

PostService::PostService(
    const repositories::PostRepository& repo,
    const services::OrganizationService& org_service,
    const services::UserService& user_service
)
    : repository_(repo), user_service_(user_service), org_service_(org_service) {}

dto::PostDTO PostService::Create(int64_t user_id, const dto::PostCreateDTO& dto) const {
    auto user_org_id = user_service_.GetOrganizationId(user_id);
    if (!user_org_id) {
        throw ForbiddenException();
    }
    (void)org_service_.Get(*user_org_id);
    models::Post post = repository_.Create(models::Post::from_create_dto(*user_org_id, dto));
    return models::Post::to_dto(post);
}

dto::PostDTO PostService::Get(int64_t id) const {
    auto post = repository_.GetById(id);
    if (!post) {
        throw PostNotFoundException();
    }
    return models::Post::to_dto(*post);
}

dto::PostListDTO PostService::GetByOrganizationPaginated(int64_t org_id, int page) const {
    static constexpr int kPageSize = 20;

    (void)org_service_.Get(org_id);

    auto [posts, total_count] = repository_.GetByOrganizationIdPaginated(org_id, page, kPageSize);

    std::vector<dto::PostDTO> items;
    items.reserve(posts.size());
    std::transform(
        std::make_move_iterator(posts.begin()),
        std::make_move_iterator(posts.end()),
        std::back_inserter(items),
        [](models::Post&& post) { return models::Post::to_dto(post); }
    );

    const std::int64_t total_pages = total_count == 0 ? 1 : (total_count + kPageSize - 1) / kPageSize;
    return {std::move(items), page, kPageSize, total_count, total_pages};
}

dto::PostDTO PostService::Update(const int64_t user_id, const int64_t post_id, const dto::PostUpdateDTO& dto) const {
    auto existing = repository_.GetById(post_id);
    if (!existing) {
        throw PostNotFoundException();
    }
    auto user_org_id = user_service_.GetOrganizationId(user_id);
    if (!user_org_id || *user_org_id != existing->organization_id) {
        throw ForbiddenException();
    }
    models::Post post = models::Post::from_update_dto(post_id, dto);
    auto result = repository_.Update(post);
    if (!result) {
        throw PostNotFoundException();
    }
    return models::Post::to_dto(*result);
}

void PostService::Delete(int64_t user_id, int64_t post_id) const {
    auto existing = repository_.GetById(post_id);
    if (!existing) {
        throw PostNotFoundException();
    }
    auto user_org_id = user_service_.GetOrganizationId(user_id);
    if (!user_org_id || *user_org_id != existing->organization_id) {
        throw ForbiddenException();
    }
    auto deleted = repository_.Delete(post_id);
    if (!deleted) {
        throw PostNotFoundException();
    }
}

}  // namespace pawspective::services
