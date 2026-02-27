#include "components/user_service_component.hpp"

#include "user_repository_component.hpp"

namespace pawspective::components {
    UserServiceComponent::UserServiceComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    ) : LoggableComponentBase(config, context),
        service_(context.FindComponent<UserRepositoryComponent>().get_repository()) {}

    const services::UserService& UserServiceComponent::get_service() const {
        return service_;
    }

} // namespace pawspective::components
