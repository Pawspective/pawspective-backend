#include "redis_session_component.hpp"
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/redis/component.hpp>
#include "jwt_component.hpp"
#include "redis_session_service.hpp"
#include "session_service.hpp"

namespace pawspective::components {
RedisSessionComponent::RedisSessionComponent(
    const userver::components::ComponentConfig &config,
    const userver::components::ComponentContext &context
)
    : LoggableComponentBase(config, context),
      session_service_(
          context.FindComponent<userver::components::Redis>("redis-session")
              .GetClient("default"),
          context.FindComponent<JwtComponent>().get_service()
      ) {
}

const services::SessionService &RedisSessionComponent::get_service() const {
    return session_service_;
}
}  // namespace pawspective::components