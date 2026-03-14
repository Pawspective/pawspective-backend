#include "animal_service_component.hpp"

#include "animal_repository_component.hpp"
#include "breed_service_component.hpp"
#include "components/user_service_component.hpp"
#include "organization_service_component.hpp"

namespace pawspective::components {
AnimalServiceComponent::AnimalServiceComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : LoggableComponentBase(config, context),
      service_(
          context.FindComponent<AnimalRepositoryComponent>().get_repository(),
          context.FindComponent<BreedServiceComponent>().get_service(),
          context.FindComponent<OrganizationServiceComponent>().get_service(),
          context.FindComponent<UserServiceComponent>().get_service()
      ) {}

const services::AnimalService& AnimalServiceComponent::get_service() const { return service_; }

}  // namespace pawspective::components
