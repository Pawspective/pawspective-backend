#include "organization_service_component.hpp"

#include "city_service_component.hpp"
#include "components/user_service_component.hpp"
#include "organization_repository_component.hpp"
#include "s3_component.hpp"

namespace pawspective::components {
OrganizationServiceComponent::OrganizationServiceComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : LoggableComponentBase(config, context),
      service_(
          context.FindComponent<OrganizationRepositoryComponent>().get_repository(),
          context.FindComponent<CityServiceComponent>().get_service(),
          context.FindComponent<UserServiceComponent>().get_service(),
          context.FindComponent<S3Component>().GetClient()
      ) {}

const services::OrganizationService& OrganizationServiceComponent::get_service() const { return service_; }

}  // namespace pawspective::components
