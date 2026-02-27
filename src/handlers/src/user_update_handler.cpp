#include "user_update_handler.hpp"
#include <userver/components/component_context.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/server/http/http_status.hpp>
#include "components/user_service_component.hpp"
#include "user_dto.hpp"
#include "user_update_dto.hpp"

namespace pawspective::handlers {
UserUpdateHandler::UserUpdateHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : HttpHandlerBase(config, component_context),
      user_service_(component_context.FindComponent<pawspective::components::UserServiceComponent>().get_service()) {}

std::string UserUpdateHandler::
    HandleRequestThrow(const userver::server::http::HttpRequest& request, userver::server::request::RequestContext&)
        const {
    const auto& id_str = request.GetPathArg("id");
    auto user_id = std::stoll(id_str);

    auto json_body = userver::formats::json::FromString(request.RequestBody());
    auto user_update_dto = json_body.As<dto::UserUpdateDTO>();

    auto updated_user = user_service_.UpdateUser(user_id, user_update_dto);

    userver::formats::json::ValueBuilder response_builder;

    response_builder = updated_user;

    request.SetResponseStatus(userver::server::http::HttpStatus::kImATeapot);

    return userver::formats::json::ToString(response_builder.ExtractValue());
}
}  // namespace pawspective::handlers