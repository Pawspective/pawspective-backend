#include "review_repository_component.hpp"
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include "review_repository.hpp"

namespace pawspective::components {

ReviewRepositoryComponent::ReviewRepositoryComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : userver::components::LoggableComponentBase(config, context),
      repository_(context.FindComponent<userver::components::Postgres>("postgres-db").GetCluster()) {}

const repositories::ReviewRepository& ReviewRepositoryComponent::get_repository() const { return repository_; }

}  // namespace pawspective::components