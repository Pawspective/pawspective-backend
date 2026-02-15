#pragma once

#include <userver/components/component.hpp>
#include <userver/server/handlers/auth/auth_checker_base.hpp>
#include <userver/server/handlers/auth/auth_checker_settings.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include "../../services/include/jwt_service.hpp"

// #include "../../services/include/user_service.hpp" // uncomment when
// UserServise will be done

namespace pawspective::handlers {
class AuthMeChecker final
    : public userver::server::handlers::auth::AuthCheckerBase {
public:
    using AuthCheckerResult = userver::server::handlers::auth::AuthCheckResult;
    explicit AuthMeChecker(services::JwtService &jwt_service);

    [[nodiscard]] AuthCheckerResult CheckAuth(
        const userver::server::http::HttpRequest &request,
        userver::server::request::RequestContext &request_context
    ) const override;
    [[nodiscard]] bool SupportsUserAuth() const noexcept override;

private:
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
    services::JwtService &jwt_service_;
};

class AuthMeHandler final
    : public userver::server::handlers::HttpHandlerJsonBase {
public:
    constexpr static std::string_view kName = "handler-auth-me";
    AuthMeHandler(
        const userver::components::ComponentConfig &config,
        const userver::components::ComponentContext &component_context,
        bool is_monitor = false
    );
    userver::formats::json::Value HandleRequestJsonThrow(
        const userver::server::http::HttpRequest &request,
        const userver::formats::json::Value &request_json,
        userver::server::request::RequestContext &context
    ) const override;

private:
    services::JwtService &jwt_service_;
    // services::UserService& user_service_; // uncomment when UserServise will
    // be done
};

}  // namespace pawspective::handlers