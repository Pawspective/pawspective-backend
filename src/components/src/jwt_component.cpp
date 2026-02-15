#include "jwt_component.hpp"
#include <chrono>
#include <string>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/secdist.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include <userver/yaml_config/schema.hpp>
#include "jwt_secrets.hpp"
#include "jwt_service.hpp"

namespace pawspective::components {
JwtComponent::JwtComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : LoggableComponentBase(config, context), service_(MakeService(config, context)) {}

const services::JwtService& JwtComponent::get_service() const { return service_; }

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

services::JwtService JwtComponent::MakeService(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
) {
    const auto& secdist = context.FindComponent<userver::components::Secdist>().Get();
    const auto& secrets = secdist.Get<models::JwtSecrets>();

    return services::JwtService{services::JwtService::Config{
        secrets.secret_key,
        config["access_ttl"].As<std::chrono::seconds>(),
        config["refresh_ttl"].As<std::chrono::seconds>()
    }};
}

}  // namespace pawspective::components