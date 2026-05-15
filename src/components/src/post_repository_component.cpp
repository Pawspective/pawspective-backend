#include "../include/post_repository_component.hpp"
#include <userver/storages/postgres/component.hpp>

#include "../../repositories/include/post_repository.hpp"

namespace pawspective::components {

PostRepositoryComponent::PostRepositoryComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : userver::components::ComponentBase(config, context),
      repository_(context.FindComponent<userver::components::Postgres>("postgres-db").GetCluster()) {}

const repositories::PostRepository& PostRepositoryComponent::get_repository() const { return repository_; }

}  // namespace pawspective::components
