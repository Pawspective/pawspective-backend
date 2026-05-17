#include "review_service_component.hpp"

#include "animal_service_component.hpp"
#include "organization_service_component.hpp"
#include "review_repository_component.hpp"

namespace pawspective::components {
ReviewServiceComponent::ReviewServiceComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : LoggableComponentBase(config, context),
      service_(
          context.FindComponent<ReviewRepositoryComponent>().get_repository(),
          context.FindComponent<AnimalServiceComponent>().get_service(),
          context.FindComponent<OrganizationServiceComponent>().get_service()
      ) {}

const services::ReviewService& ReviewServiceComponent::get_service() const { return service_; }

}  // namespace pawspective::components
