#include "../include/auth_me_handler.hpp"
#include <userver/components/component.hpp>
#include <userver/http/common_headers.hpp>
#include "../../services/include/jwt_service.hpp"

namespace pawspective::handlers {

AuthMeChecker::AuthMeChecker(services::JwtService &jwt_service)
    : jwt_service_(jwt_service) {
}

AuthMeChecker::AuthCheckerResult AuthMeChecker::CheckAuth(
    const userver::server::http::HttpRequest &request,
    userver::server::request::RequestContext &request_context
) const {
    const auto &auth_header =
        request.GetHeader(userver::http::headers::kAuthorization);
    if (auth_header.empty()) {
        return AuthCheckerResult{
            AuthCheckerResult::Status::kTokenNotFound,
            {},
            "Empty Authorization header",
            userver::server::handlers::HandlerErrorCode::kUnauthorized
        };
    }

    const auto bearer_sep_pos = auth_header.find(' ');
    if (bearer_sep_pos == std::string::npos ||
        auth_header.substr(0, bearer_sep_pos) != "Bearer") {
        return AuthCheckerResult{
            AuthCheckerResult::Status::kTokenNotFound,
            {},
            "Invalid Authorization header format. Expected: Bearer <token>",
            userver::server::handlers::HandlerErrorCode::kUnauthorized
        };
    }
    std::string token = auth_header.substr(bearer_sep_pos + 1);
    auto payload = jwt_service_.validate_access_token(token);
    if (!payload) {
        return AuthCheckerResult{
            AuthCheckerResult::Status::kForbidden,
            {},
            "Invalid or expired access token",
            userver::server::handlers::HandlerErrorCode::kUnauthorized
        };
    }

    request_context.SetData("user_id", payload->user_id);
    return {};
}

bool AuthMeChecker::SupportsUserAuth() const noexcept {
    return true;
}

AuthMeHandler::AuthMeHandler(
    const userver::components::ComponentConfig &config,
    const userver::components::ComponentContext &component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      jwt_service_(component_context.FindComponent<services::JwtService>(
      ) /*,
           user_service_(component_context.FindComponent<services::UserService>())*/
      ) {
}

userver::formats::json::Value AuthMeHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest &request,
    const userver::formats::json::Value &,
    userver::server::request::RequestContext &context
) const {
    const auto &user_id = context.GetData<std::string>("user_id");
    // auto user = user_service_.GetUserById(user_id);
    userver::formats::json::ValueBuilder response;
    response["id"] = user_id;
    /*response["email"] = user->email;
    response["first_name"] = user->first_name;
    response["last_name"] = user->last_name;
    response["organization_id"] = user->organization_id;*/
    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
    return response.ExtractValue();
}

}  // namespace pawspective::handlers
