#pragma once

#include <userver/server/handlers/http_handler_json_base.hpp>
#include "organization_service_component.hpp"

namespace pawspective::handlers {

class OrgRegistrationHandler final : public userver::server::handlers::HttpHandlerJsonBase {
public:
    constexpr static std::string_view kName = "handler-org-registration";

    OrgRegistrationHandler(
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
    const components::OrganizationServiceComponent& org_service_;
};

}  // namespace pawspective::handlers
