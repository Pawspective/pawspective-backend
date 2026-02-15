#include "pg_session_component.hpp"
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include "jwt_component.hpp"
#include "pg_session_service.hpp"
#include "session_service.hpp"

namespace pawspective::components {
PgSessionComponent::PgSessionComponent(
    const userver::components::ComponentConfig &config,
    const userver::components::ComponentContext &context
)
    : LoggableComponentBase(config, context),
      session_service_(
          context.FindComponent<userver::components::Postgres>("postgres-db")
              .GetCluster(),
          context.FindComponent<JwtComponent>().get_service()
      ) {
}

const services::SessionService &PgSessionComponent::get_service() const {
    return session_service_;
}
}  // namespace pawspective::components