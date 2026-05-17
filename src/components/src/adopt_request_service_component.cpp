#include "adopt_request_service_component.hpp"

#include "adopt_request_repository_component.hpp"
#include "animal_service_component.hpp"
#include "components/user_service_component.hpp"

namespace pawspective::components {

AdoptRequestServiceComponent::AdoptRequestServiceComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : LoggableComponentBase(config, context),
      service_(
          context.FindComponent<AdoptRequestRepositoryComponent>().get_repository(),
          context.FindComponent<AnimalServiceComponent>().get_service(),
          context.FindComponent<UserServiceComponent>().get_service()
      ) {}

const services::AdoptRequestService& AdoptRequestServiceComponent::get_service() const { return service_; }

}  // namespace pawspective::components
