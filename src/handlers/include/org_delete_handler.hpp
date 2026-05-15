#pragma once

#include <userver/server/handlers/http_handler_json_base.hpp>
#include "organization_service_component.hpp"

namespace pawspective::handlers {

class OrganizationDeleteHandler final : public userver::server::handlers::HttpHandlerJsonBase {
public:
    constexpr static std::string_view kName = "handler-org-delete";

    OrganizationDeleteHandler(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context
    );

    userver::formats::json::Value HandleRequestJsonThrow(
        const userver::server::http::HttpRequest& request,
        const userver::formats::json::Value& request_json,
        userver::server::request::RequestContext& context
    ) const override;

private:
    const components::OrganizationServiceComponent& organization_service_;
};
}  // namespace pawspective::handlers
