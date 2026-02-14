#pragma once

#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/components/loggable_component_base.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include "jwt_service.hpp"

namespace pawspective::components {
class JwtComponent final : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "jwt-service";

    explicit JwtComponent(
        const userver::components::ComponentConfig &config,
        const userver::components::ComponentContext &context
    );

    [[nodiscard]] const services::JwtService &get_service() const {
        return service_;
    }

    static userver::yaml_config::Schema GetStaticConfigSchema();

private:
    services::JwtService service_;
};
}  // namespace pawspective::components