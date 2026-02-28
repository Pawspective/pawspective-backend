#pragma once

#include <userver/server/handlers/http_handler_json_base.hpp>
#include "services/user_service.hpp"

namespace pawspective::handlers {

class UserUpdateHandler final : public userver::server::handlers::HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-user-update";

    UserUpdateHandler(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    userver::formats::json::Value HandleRequestJsonThrow(
        const userver::server::http::HttpRequest& request,
        const userver::formats::json::Value& request_body,
        userver::server::request::RequestContext& context
    ) const override;

private:
    const services::UserService& user_service_;
};

}  // namespace pawspective::handlers
