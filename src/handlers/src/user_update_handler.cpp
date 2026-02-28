#include "user_update_handler.hpp"
#include <userver/components/component_context.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/server/http/http_status.hpp>
#include "components/user_service_component.hpp"
#include "user_update_dto.hpp"

namespace pawspective::handlers {
UserUpdateHandler::UserUpdateHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : HttpHandlerJsonBase(config, component_context),
      user_service_(component_context.FindComponent<pawspective::components::UserServiceComponent>().get_service()) {}

userver::formats::json::
    Value
    UserUpdateHandler::
        HandleRequestJsonThrow(const userver::server::http::HttpRequest& request, const userver::formats::json::Value& request_body, userver::server::request::RequestContext&)
            const {
    const auto& id_str = request.GetPathArg("id");
    auto user_id = std::stoll(id_str);

    auto user_update_dot = request_body.As<dto::UserUpdateDTO>();

    auto updated_user = user_service_.UpdateUser(user_id, user_update_dot);

    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);

    return userver::formats::json::ValueBuilder{updated_user}.ExtractValue();
}
}  // namespace pawspective::handlers