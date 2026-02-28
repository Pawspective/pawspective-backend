#include "../include/auth_me_handler.hpp"
#include <string>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/server/request/request_context.hpp>
#include "../../components/include/jwt_component.hpp"
#include "components/user_service_component.hpp"
#include "services/exception.hpp"
#include "utils/error_response.hpp"

// #include "../../dto/include/user_dto.hpp"

namespace pawspective::handlers {

AuthMeHandler::AuthMeHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      jwt_service_(component_context.FindComponent<components::JwtComponent>().get_service()),
      user_service_(component_context.FindComponent<components::UserServiceComponent>().get_service())

{}

userver::formats::json::Value AuthMeHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value&,
    userver::server::request::RequestContext& context
) const {
    const auto& user_id = context.GetData<int64_t>("user_id");
    models::User user;
    try {
        user = user_service_.GetUserById(user_id);
    } catch (const services::UserNotFoundException& e) {
        LOG_WARNING() << "User not found for id: " << user_id;
        throw userver::server::handlers::ResourceNotFound{userver::server::handlers::ExternalBody{
            utils::ErrorResponse(utils::error_code::kUserNotFound, "User not found").GetString()
        }};
    }

    userver::formats::json::ValueBuilder response = models::User::to_dto(user);
    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
    return response.ExtractValue();
}

}  // namespace pawspective::handlers
