#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/clients/http/middlewares/pipeline_component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>
#include "auth_me_handler.hpp"
#include "health_handler.hpp"
#include "jwt_component.hpp"
#include "pg_session_component.hpp"

int main(int argc, char *argv[]) {
    auto component_list =
        userver::components::MinimalServerComponentList()
            .Append<userver::components::TestsuiteSupport>()
            .Append<userver::components::HttpClientCore>()  // NOLINT
            .Append<userver::components::HttpClient>()      // NOLINT
            .Append<userver::clients::http::MiddlewarePipelineComponent>()
            .Append<userver::clients::dns::Component>()
            .Append<pawspective::handlers::HealthCheck>()
            .Append<pawspective::handlers::AuthMeHandler>()
            .Append<userver::components::DefaultSecdistProvider>()
            .Append<userver::components::Secdist>()
            .Append<userver::components::Postgres>("postgres-db")
            .Append<pawspective::components::PgSessionComponent>()
            .Append<pawspective::components::JwtComponent>();

    return userver::utils::DaemonMain(argc, argv, component_list);
}