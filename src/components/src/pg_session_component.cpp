#include "pg_session_component.hpp"
#include <memory>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <utility>
#include "jwt_component.hpp"
#include "pg_session_service.hpp"

namespace pawspective::components {
PgSessionComponent::PgSessionComponent(
    const userver::components::ComponentConfig &config,
    const userver::components::ComponentContext &context
)
    : LoggableComponentBase(config, context) {
    auto pg_cluster =
        context.FindComponent<userver::components::Postgres>("postgres-db")
            .GetCluster();
    const auto &jwt_service =
        context.FindComponent<JwtComponent>().get_service();

    impl_ = std::make_unique<services::PgSessionService>(
        std::move(pg_cluster), jwt_service
    );
}
}  // namespace pawspective::components