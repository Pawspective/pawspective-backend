#include "organization_service_component.hpp"

#include "organization_repository_component.hpp"

namespace pawspective::components {
OrganizationServiceComponent::OrganizationServiceComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : LoggableComponentBase(config, context),
      service_(context.FindComponent<OrganizationRepositoryComponent>().get_repository()) {}

const services::OrganizationService& OrganizationServiceComponent::get_service() const { return service_; }

}  // namespace pawspective::components
