#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/clients/http/middlewares/pipeline_component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/auth/auth_checker_factory.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>
#include "adopt_request_accept_handler.hpp"
#include "adopt_request_deny_handler.hpp"
#include "adopt_request_repository_component.hpp"
#include "adopt_request_service_component.hpp"
#include "adopt_requests_get_handler.hpp"
#include "animal_adopt_handler.hpp"
#include "animal_delete_handler.hpp"
#include "animal_filters_handler.hpp"
#include "animal_get_handler.hpp"
#include "animal_list_handler.hpp"
#include "animal_registration_handler.hpp"
#include "animal_repository_component.hpp"
#include "animal_service_component.hpp"
#include "animal_update_handler.hpp"
#include "auth_checker.hpp"
#include "auth_login_handler.hpp"
#include "auth_logout_handler.hpp"
#include "auth_me_handler.hpp"
#include "auth_refresh_handler.hpp"
#include "breed_list_handler.hpp"
#include "breed_repository_component.hpp"
#include "breed_service_component.hpp"
#include "city_list_handler.hpp"
#include "city_repository_component.hpp"
#include "city_service_component.hpp"
#include "components/user_service_component.hpp"
#include "health_handler.hpp"
#include "jwt_component.hpp"
#include "me_adopted_animals_pending_review_handler.hpp"
#include "org_animals_handler.hpp"
#include "org_delete_handler.hpp"
#include "org_get_handler.hpp"
#include "org_registration_handler.hpp"
#include "org_search_handler.hpp"
#include "org_update_handler.hpp"
#include "organization_repository_component.hpp"
#include "organization_service_component.hpp"
#include "pg_session_component.hpp"
#include "post_create_handler.hpp"
#include "post_delete_handler.hpp"
#include "post_repository_component.hpp"
#include "post_service_component.hpp"
#include "post_update_handler.hpp"
#include "posts_get_handler.hpp"
#include "review_create_handler.hpp"
#include "review_delete_handler.hpp"
#include "review_get_handler.hpp"
#include "review_repository_component.hpp"
#include "review_service_component.hpp"
#include "review_update_handler.hpp"
#include "session_cleaner_component.hpp"
#include "user_delete_handler.hpp"
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
            .Append<userver::server::handlers::TestsControl>()
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
            .Append<pawspective::components::OrganizationServiceComponent>()
            .Append<pawspective::components::UserServiceComponent>()
            .Append<pawspective::handlers::UserUpdateHandler>()
            .Append<pawspective::components::CityRepositoryComponent>()
            .Append<pawspective::components::CityServiceComponent>()
            .Append<pawspective::components::OrganizationRepositoryComponent>()
            .Append<pawspective::components::PostRepositoryComponent>()
            .Append<pawspective::components::ReviewRepositoryComponent>()
            .Append<pawspective::components::ReviewServiceComponent>()
            .Append<pawspective::components::AdoptRequestRepositoryComponent>()
            .Append<pawspective::components::AdoptRequestServiceComponent>()
            .Append<pawspective::components::AnimalRepositoryComponent>()
            .Append<pawspective::components::AnimalServiceComponent>()
            .Append<pawspective::components::BreedRepositoryComponent>()
            .Append<pawspective::components::BreedServiceComponent>()
            .Append<pawspective::components::SessionCleanerComponent>()
            .Append<pawspective::components::PostServiceComponent>()
            .Append<pawspective::handlers::OrgRegistrationHandler>()
            .Append<pawspective::handlers::OrgSearchHandler>()
            .Append<pawspective::handlers::OrgUpdateHandler>()
            .Append<pawspective::handlers::OrganizationGetHandler>()
            .Append<pawspective::handlers::CityListHandler>()
            .Append<pawspective::handlers::AnimalRegistrationHandler>()
            .Append<pawspective::handlers::AnimalUpdateHandler>()
            .Append<pawspective::handlers::OrganizationAnimalHandler>()
            .Append<pawspective::handlers::AnimalGetHandler>()
            .Append<pawspective::handlers::AnimalListHandler>()
            .Append<pawspective::handlers::AnimalFiltersHandler>()
            .Append<pawspective::handlers::ReviewCreateHandler>()
            .Append<pawspective::handlers::MeAdoptedAnimalsPendingReviewHandler>()
            .Append<pawspective::handlers::ReviewUpdateHandler>()
            .Append<pawspective::handlers::ReviewGetHandler>()
            .Append<pawspective::handlers::ReviewDeleteHandler>()
            .Append<pawspective::handlers::UserDeleteHandler>()
            .Append<pawspective::handlers::OrganizationDeleteHandler>()
            .Append<pawspective::handlers::AnimalDeleteHandler>()
            .Append<pawspective::handlers::BreedListHandler>()
            .Append<pawspective::handlers::PostsGetHandler>()
            .Append<pawspective::handlers::PostCreateHandler>()
            .Append<pawspective::handlers::PostUpdateHandler>()
            .Append<pawspective::handlers::PostDeleteHandler>()
            .Append<pawspective::handlers::AdoptRequestsGetHandler>()
            .Append<pawspective::handlers::AdoptRequestAcceptHandler>()
            .Append<pawspective::handlers::AdoptRequestDenyHandler>()
            .Append<pawspective::handlers::AnimalAdoptHandler>();

    return userver::utils::DaemonMain(argc, argv, component_list);
}