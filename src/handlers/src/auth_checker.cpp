#include "auth_checker.hpp"
#include <memory>
#include <string>
#include <userver/components/component_context.hpp>
#include <userver/http/common_headers.hpp>
#include <userver/server/handlers/auth/auth_checker_base.hpp>
#include <userver/server/handlers/auth/handler_auth_config.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
#include "../../components/include/jwt_component.hpp"
#include "../../services/include/jwt_service.hpp"

namespace pawspective::handlers {

AuthChecker::AuthChecker(const services::JwtService& jwt_service, bool is_required)
    : jwt_service_(jwt_service), is_required_(is_required) {}

AuthChecker::AuthCheckResult AuthChecker::CheckAuth(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& request_context
) const {
    const auto& auth_value = request.GetHeader(userver::http::headers::kAuthorization);

    if (auth_value.empty()) {
        if (!is_required_) {
            return {};
        }
        return AuthCheckResult{
            AuthCheckResult::Status::kTokenNotFound,
            {},
            "Empty Authorization header",
            userver::server::handlers::HandlerErrorCode::kUnauthorized
        };
    }

    const auto bearer_sep_pos = auth_value.find(' ');
    if (bearer_sep_pos == std::string::npos || auth_value.substr(0, bearer_sep_pos) != "Bearer") {
        return AuthCheckResult{
            AuthCheckResult::Status::kTokenNotFound,
            {},
            "Invalid Authorization header format. Expected: Bearer <token>",
            userver::server::handlers::HandlerErrorCode::kUnauthorized
        };
    }
    const std::string token = auth_value.substr(bearer_sep_pos + 1);
    auto payload = jwt_service_.validate_access_token(token);
    if (!payload) {
        return AuthCheckResult{
            AuthCheckResult::Status::kForbidden,
            {},
            "Invalid or expired access token",
            userver::server::handlers::HandlerErrorCode::kUnauthorized
        };
    }

    request_context.SetData("user_id", payload->user_id);
    return {};
}

bool AuthChecker::SupportsUserAuth() const noexcept { return true; }

AuthCheckerFactory::AuthCheckerFactory(const userver::components::ComponentContext& context)
    : jwt_service_(context.FindComponent<components::JwtComponent>().get_service()) {}

userver::server::handlers::auth::AuthCheckerBasePtr AuthCheckerFactory::MakeAuthChecker(
    const userver::server::handlers::auth::HandlerAuthConfig& auth_config
) const {
    auto is_required = auth_config["required"].As<bool>(false);
    return std::make_shared<AuthChecker>(jwt_service_, is_required);
}

}  // namespace pawspective::handlers