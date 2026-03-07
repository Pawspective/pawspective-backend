#include "organization_repository_component.hpp"
#include <userver/storages/postgres/component.hpp>

#include "organization_repository.hpp"

namespace pawspective::components {

OrganizationRepositoryComponent::OrganizationRepositoryComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : userver::components::ComponentBase(config, context),
      repository_(context.FindComponent<userver::components::Postgres>("postgres-db").GetCluster()) {}

const repositories::OrganizationRepository& OrganizationRepositoryComponent::get_repository() const {
    return repository_;
}

}  // namespace pawspective::components
