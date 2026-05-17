#include "adopt_request_repository_component.hpp"

#include <userver/storages/postgres/component.hpp>

namespace pawspective::components {

AdoptRequestRepositoryComponent::AdoptRequestRepositoryComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : userver::components::ComponentBase(config, context),
      repository_(context.FindComponent<userver::components::Postgres>("postgres-db").GetCluster()) {}

const repositories::AdoptRequestRepository& AdoptRequestRepositoryComponent::get_repository() const {
    return repository_;
}

}  // namespace pawspective::components
