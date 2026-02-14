#include "jwt_component.hpp"
#include <chrono>
#include <string>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include <userver/yaml_config/schema.hpp>
#include "jwt_service.hpp"

namespace pawspective::components {
JwtComponent::JwtComponent(
    const userver::components::ComponentConfig &config,
    const userver::components::ComponentContext &context
)
    : LoggableComponentBase(config, context),
      service_(services::JwtService::Config{
          config["secret_key"].As<std::string>(),
          config["access_ttl"].As<std::chrono::seconds>(),
          config["refresh_ttl"].As<std::chrono::seconds>()
      }) {
}

const services::JwtService &JwtComponent::get_service() const {
    return service_;
}

userver::yaml_config::Schema JwtComponent::GetStaticConfigSchema() {
    return userver::yaml_config::MergeSchemas<LoggableComponentBase>(R"(
        type: object
        description: JWT service configuration
        additionalProperties: false
        properties:
            secret_key:
                type: string
                description: Secret key for signing JWT tokens
            access_ttl:
                type: integer
                description: Time-to-live for access tokens in seconds
            refresh_ttl:
                type: integer
                description: Time-to-live for refresh tokens in seconds
    )");
}
}  // namespace pawspective::components