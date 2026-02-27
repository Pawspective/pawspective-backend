#pragma once

#include <userver/server/handlers/http_handler_base.hpp>
#include "services/user_service.hpp"

namespace pawspective::handlers {
class UserUpdateHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-user-update";

    UserUpdateHandler(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context
    ) const override;

private:
    const services::UserService& user_service_;
};
}  // namespace pawspective::handlers