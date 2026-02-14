#include "redis_session_component.hpp"
#include <memory>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/redis/component.hpp>
#include <utility>
#include "jwt_component.hpp"
#include "redis_session_service.hpp"

namespace pawspective::components {
RedisSessionComponent::RedisSessionComponent(
    const userver::components::ComponentConfig &config,
    const userver::components::ComponentContext &context
)
    : LoggableComponentBase(config, context) {
    auto redis_client =
        context.FindComponent<userver::components::Redis>("redis-session")
            .GetClient("default");
    const auto &jwt_service =
        context.FindComponent<JwtComponent>().get_service();

    impl_ = std::make_unique<services::RedisSessionService>(
        std::move(redis_client), jwt_service
    );
}
}  // namespace pawspective::components