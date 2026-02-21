#include "user_repository_component.hpp"
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include "../../repositories/include/user_repository.hpp"

namespace pawspective::components {

UserRepositoryComponent::UserRepositoryComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : userver::components::ComponentBase(config, context),
      repository_(context.FindComponent<userver::components::Postgres>("postgres-db").GetCluster()) {}

const repositories::UserRepository& UserRepositoryComponent::get_repository() const { return repository_; }
}  // namespace pawspective::components