#include "post_service_component.hpp"
#include "components/user_service_component.hpp"
#include "organization_service_component.hpp"
#include "post_repository_component.hpp"

namespace pawspective::components {

PostServiceComponent::PostServiceComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : userver::components::LoggableComponentBase(config, context),
      service_(
          context.FindComponent<PostRepositoryComponent>().get_repository(),
          context.FindComponent<OrganizationServiceComponent>().get_service(),
          context.FindComponent<UserServiceComponent>().get_service()
      ) {}

const services::PostService& PostServiceComponent::get_service() const { return service_; }

}  // namespace pawspective::components