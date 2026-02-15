#pragma once
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/components/loggable_component_base.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include "pg_session_service.hpp"
#include "session_service.hpp"

namespace pawspective::components {
class PgSessionComponent final : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "session-service-pg";
    explicit PgSessionComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    [[nodiscard]] const services::SessionService& get_service() const;

private:
    services::PgSessionService session_service_;
};
}  // namespace pawspective::components