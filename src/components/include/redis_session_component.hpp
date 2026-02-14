#pragma once
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/components/loggable_component_base.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include "redis_session_service.hpp"
#include "session_service.hpp"

namespace pawspective::components {
class RedisSessionComponent final
    : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "session-service-redis";
    explicit RedisSessionComponent(
        const userver::components::ComponentConfig &config,
        const userver::components::ComponentContext &context
    );

    [[nodiscard]] services::SessionService &get_service() {
        return *impl_;
    }

private:
    std::unique_ptr<services::RedisSessionService> impl_;
};
}  // namespace pawspective::components