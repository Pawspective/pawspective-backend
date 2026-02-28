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

    std::int64_t user_id = 0;
    try {
        user_id = std::stoll(id_str);

    } catch (const std::exception& e) {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{"Invalid user ID format"});
    }

    try {
        auto update_dto = request_body.As<pawspective::dto::UserUpdateDTO>();

        auto updated_model = user_service_.UpdateUser(user_id, update_dto);

        auto response_dto = models::User::to_dto(updated_model);

        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);

        return userver::formats::json::ValueBuilder(response_dto).ExtractValue();
    } catch (const std::exception& e) {
        throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{
            "Failed to update user: " + std::string(e.what())
        });
    }
}
}  // namespace pawspective::handlers