#include "city_service_component.hpp"

#include "city_repository_component.hpp"

namespace pawspective::components {

CityServiceComponent::CityServiceComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : LoggableComponentBase(config, context),
      service_(context.FindComponent<CityRepositoryComponent>().get_repository()) {}

const services::CityService& CityServiceComponent::get_service() const { return service_; }

}  // namespace pawspective::components
