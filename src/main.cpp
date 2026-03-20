#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/clients/http/middlewares/pipeline_component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/auth/auth_checker_factory.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>
#include "animal_registration_handler.hpp"
#include "animal_repository_component.hpp"
#include "animal_service_component.hpp"
#include "animal_update_handler.hpp"
#include "auth_checker.hpp"
#include "auth_login_handler.hpp"
#include "auth_logout_handler.hpp"
#include "auth_me_handler.hpp"
#include "auth_refresh_handler.hpp"
#include "breed_repository_component.hpp"
#include "breed_service_component.hpp"
#include "city_list_handler.hpp"
#include "city_repository_component.hpp"
#include "city_service_component.hpp"
#include "components/user_service_component.hpp"
#include "health_handler.hpp"
#include "jwt_component.hpp"
#include "org_animals_handler.hpp"
#include "org_get_handler.hpp"
#include "org_registration_handler.hpp"
#include "org_search_handler.hpp"
#include "org_update_handler.hpp"
#include "organization_repository_component.hpp"
#include "organization_service_component.hpp"
#include "pg_session_component.hpp"
#include "user_registration_handler.hpp"
#include "user_repository_component.hpp"
#include "user_update_handler.hpp"

int main(int argc, char* argv[]) {
    userver::server::handlers::auth::RegisterAuthCheckerFactory<pawspective::handlers::AuthCheckerFactory>();
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
            .Append<pawspective::components::JwtComponent>()
            .Append<pawspective::components::UserRepositoryComponent>()
            .Append<pawspective::handlers::AuthRefreshHandler>()
            .Append<pawspective::handlers::UserRegistrationHandler>()
            .Append<pawspective::handlers::AuthLogoutHandler>()
            .Append<pawspective::handlers::AuthLoginHandler>()
            .Append<pawspective::components::UserServiceComponent>()
            .Append<pawspective::handlers::UserUpdateHandler>()
            .Append<pawspective::components::CityRepositoryComponent>()
            .Append<pawspective::components::CityServiceComponent>()
            .Append<pawspective::components::OrganizationRepositoryComponent>()
            .Append<pawspective::components::OrganizationServiceComponent>()
            .Append<pawspective::components::AnimalRepositoryComponent>()
            .Append<pawspective::components::AnimalServiceComponent>()
            .Append<pawspective::components::BreedRepositoryComponent>()
            .Append<pawspective::components::BreedServiceComponent>()
            .Append<pawspective::handlers::OrgRegistrationHandler>()
            .Append<pawspective::handlers::OrgSearchHandler>()
            .Append<pawspective::handlers::OrgUpdateHandler>()
            .Append<pawspective::handlers::OrganizationGetHandler>()
            .Append<pawspective::handlers::CityListHandler>()
            .Append<pawspective::handlers::AnimalRegistrationHandler>()
            .Append<pawspective::handlers::AnimalUpdateHandler>()
            .Append<pawspective::handlers::OrganizationAnimalHandler>();

    return userver::utils::DaemonMain(argc, argv, component_list);
}