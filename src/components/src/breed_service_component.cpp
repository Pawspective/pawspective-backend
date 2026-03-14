#include "breed_service_component.hpp"

#include "breed_repository_component.hpp"

namespace pawspective::components {

BreedServiceComponent::BreedServiceComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : LoggableComponentBase(config, context),
      service_(context.FindComponent<BreedRepositoryComponent>().get_repository()) {}

const services::BreedService& BreedServiceComponent::get_service() const { return service_; }

}  // namespace pawspective::components
