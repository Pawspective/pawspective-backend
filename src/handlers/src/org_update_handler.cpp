#include "../include/org_update_handler.hpp"

#include <cctype>
#include <userver/components/component_context.hpp>
#include <userver/formats/json/exception.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/utils/regex.hpp>
#include "../../components/include/organization_service_component.hpp"
#include "organization_update_dto.hpp"
#include "services/exception.hpp"
#include "utils/error_response.hpp"
#include "utils/exception.hpp"
#include "utils/validator.hpp"

namespace pawspective::handlers {

OrgUpdateHandler::OrgUpdateHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context
)
    : HttpHandlerJsonBase(config, component_context),
      organization_service_(component_context.FindComponent<components::OrganizationServiceComponent>().get_service()) {
}

userver::formats::json::Value OrgUpdateHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_body,
    userver::server::request::RequestContext& context
) const {
    const auto& user_id = context.GetData<int64_t>("user_id");
    const auto& id_str = request.GetPathArg("id");
    std::int64_t org_id = 0;

    try {
        org_id = std::stoll(id_str);
    } catch (const std::exception& e) {
        LOG_WARNING() << "Invalid organization ID format in path: " << id_str;
        utils::ErrorResponse(utils::error_code::kOrganizationNotFound, "Invalid organization ID format")
            .ThrowClientError();
    }

    try {
        auto update_dto = request_body.As<dto::OrganizationUpdateDTO>();

        utils::Validator validator;
        if (update_dto.name.has_value()) {
            validator.Field("name", *update_dto.name).NotBlank().MaxLength(255);
        }
        if (update_dto.avatar_url.has_value()) {
            static const userver::utils::regex kPureFilenameRegex(R"(^[a-zA-Z0-9]+\.(jpg|jpeg|png|webp|gif)$)");
            validator.Field("avatar_url", *update_dto.avatar_url)
                .MaxLength(2000)
                .Matches(kPureFilenameRegex, "must be a valid photo filename (e.g. 66035e3bf.jpg)");
        }
        if (update_dto.description.has_value()) {
            validator.Field("description", *update_dto.description).MaxLength(2000);
        }
        validator.ThrowIfInvalid();

        LOG_INFO() << "Updating organization " << org_id;
        auto updated_org = organization_service_.Update(user_id, org_id, update_dto);

        LOG_INFO() << "Organization " << org_id << " updated successfully";
        return userver::formats::json::ValueBuilder(updated_org).ExtractValue();

    } catch (const userver::formats::json::MemberMissingException& e) {
        utils::ErrorResponse(utils::error_code::kMissingField, "Missing required field").ThrowClientError();
    } catch (const userver::formats::json::Exception& e) {
        utils::ErrorResponse(utils::error_code::kInvalidJsonFormat, "Invalid JSON format").ThrowClientError();
    } catch (const utils::ValidationException& e) {
        e.ThrowClientError();
    } catch (const services::OrganizationNotFoundException& e) {
        LOG_WARNING() << "Organization not found: " << org_id;
        utils::ErrorResponse(utils::error_code::kValidationError, "Organization not found").ThrowNotFound();
    } catch (const services::CityNotFoundException& e) {
        utils::ErrorResponse(utils::error_code::kCityNotFound, "City not found").ThrowNotFound();
    } catch (const services::ForbiddenException& e) {
        LOG_WARNING() << "User " << user_id << " tried to update organization " << org_id << " without permission";
        utils::ErrorResponse(utils::error_code::kForbidden, "You are not allowed to update this organization")
            .ThrowForbidden();
    }
}
}  // namespace pawspective::handlers
