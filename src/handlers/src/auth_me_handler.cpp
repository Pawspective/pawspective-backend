#include "../include/auth_me_handler.hpp"
#include <string>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/server/request/request_context.hpp>
#include "../../components/include/jwt_component.hpp"

// #include "../../dto/include/user_dto.hpp"

namespace pawspective::handlers {

AuthMeHandler::AuthMeHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      jwt_service_(component_context.FindComponent<components::JwtComponent>()
                       .get_service() /*,
                                         user_service_(component_context.FindComponent<services::UserService>())*/
      ) {}

userver::formats::json::Value AuthMeHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value&,
    userver::server::request::RequestContext& context
) const {
    const auto& user_id = context.GetData<std::string>("user_id");
    // auto user = user_service_.GetUserById(user_id);
    userver::formats::json::ValueBuilder response;  // TODO: delete when UserService will be done
    response["id"] = user_id;                       // TODO: delete when UserService will be done
    /* dto::UserDTO user_dto;
    user_dto.id = user->id;
    user_dto.email = user->email;
    user_dto.first_name = user->first_name;
    user_dto.last_name = user->last_name;
    user_dto.organization_id = user->organization_id;*/
    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
    return response.ExtractValue();  // TODO: delete when UserService will be done
    // return Serialize(user_dto,
    // userver::formats::serialize::To<userver::formats::json::Value>());
}

}  // namespace pawspective::handlers
