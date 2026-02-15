#pragma once

#include <userver/components/component.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include "../../components/include/pg_session_component.hpp"

namespace pawspective::handlers {

class AuthRefreshHandler final : public userver::server::handlers::HttpHandlerJsonBase {
public:
    constexpr static std::string_view kName = "handler-auth-refresh";

    AuthRefreshHandler(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context,
        bool is_monitor = false
    );

    userver::formats::json::Value HandleRequestJsonThrow(
        const userver::server::http::HttpRequest& request,
        const userver::formats::json::Value& request_json,
        userver::server::request::RequestContext& context
    ) const override;

private:
    const components::PgSessionComponent& session_component_;
};
}  // namespace pawspective::handlers