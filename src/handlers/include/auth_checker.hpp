#pragma once

#include <userver/components/component_context.hpp>
#include <userver/server/handlers/auth/auth_checker_base.hpp>
#include <userver/server/handlers/auth/auth_checker_factory.hpp>
#include "../../services/include/jwt_service.hpp"

namespace pawspective::handlers {

class AuthChecker final : public userver::server::handlers::auth::AuthCheckerBase {
public:
    using AuthCheckResult = userver::server::handlers::auth::AuthCheckResult;

    AuthChecker(const services::JwtService& jwt_service, bool is_required);

    [[nodiscard]] AuthCheckResult CheckAuth(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& request_context
    ) const override;

    [[nodiscard]] bool SupportsUserAuth() const noexcept override;

private:
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
    const services::JwtService& jwt_service_;
    bool is_required_;
};

class AuthCheckerFactory final : public userver::server::handlers::auth::AuthCheckerFactoryBase {
public:
    static constexpr std::string_view kAuthType = "jwt";

    explicit AuthCheckerFactory(const userver::components::ComponentContext& context);

    [[nodiscard]] userver::server::handlers::auth::AuthCheckerBasePtr MakeAuthChecker(
        const userver::server::handlers::auth::HandlerAuthConfig& auth_config
    ) const override;

private:
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
    const services::JwtService& jwt_service_;
};

}  // namespace pawspective::handlers