#pragma once

#include <string>
#include <string_view>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>

namespace pawspective::handlers {

class HealthCheck final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-health-check";

    using HttpHandlerBase::HttpHandlerBase;

    std::string HandleRequest(
        userver::server::http::HttpRequest &request,
        userver::server::request::RequestContext &context
    ) const override;
};

}  // namespace handlers::health