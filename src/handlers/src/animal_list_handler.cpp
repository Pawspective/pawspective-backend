#include "animal_list_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>  // NOLINT
#include "animal_filter_dto.hpp"
#include "animal_service_component.hpp"
#include "utils/error_response.hpp"

namespace pawspective::handlers {

namespace {

template <typename T>
T ParseEnumFromString(const std::string& str) {
    return userver::formats::json::ValueBuilder(str).ExtractValue().As<T>();
}

template <typename T>
std::optional<std::vector<T>> ParseEnumVector(const std::vector<std::string>& args) {
    if (args.empty()) {
        return std::nullopt;
    }
    std::vector<T> result;
    result.reserve(args.size());
    for (const auto& s : args) {
        result.push_back(ParseEnumFromString<T>(s));
    }
    return result;
}

std::optional<std::vector<std::int64_t>> ParseInt64Vector(
    const std::vector<std::string>& args,
    std::string_view param_name
) {
    if (args.empty()) {
        return std::nullopt;
    }
    std::vector<std::int64_t> result;
    result.reserve(args.size());
    for (const auto& s : args) {
        try {
            result.push_back(std::stoll(s));
        } catch (const std::exception&) {
            utils::ErrorResponse(
                utils::error_code::kValidationError,
                fmt::format("{} must contain only integers", param_name)
            )
                .ThrowClientError();
        }
    }
    return result;
}

std::optional<int> ParseOptionalInt(const userver::server::http::HttpRequest& request, std::string_view name) {
    if (!request.HasArg(name)) {
        return std::nullopt;
    }
    try {
        return std::stoi(std::string(request.GetArg(name)));
    } catch (const std::exception&) {
        utils::ErrorResponse(utils::error_code::kValidationError, fmt::format("{} must be an integer", name))
            .ThrowClientError();
    }
    return std::nullopt;
}

dto::AnimalFilterDTO ParseFiltersFromRequest(const userver::server::http::HttpRequest& request) {
    dto::AnimalFilterDTO dto;

    dto.breeds = ParseInt64Vector(request.GetArgVector("breeds"), "breeds");
    dto.city_ids = ParseInt64Vector(request.GetArgVector("city_ids"), "city_ids");
    dto.animal_types = ParseEnumVector<models::AnimalType>(request.GetArgVector("animal_types"));
    dto.sizes = ParseEnumVector<models::AnimalSize>(request.GetArgVector("sizes"));
    dto.genders = ParseEnumVector<models::AnimalGender>(request.GetArgVector("genders"));
    dto.care_levels = ParseEnumVector<models::CareLevel>(request.GetArgVector("care_levels"));
    dto.colors = ParseEnumVector<models::AnimalColor>(request.GetArgVector("colors"));
    dto.good_withs = ParseEnumVector<models::GoodWith>(request.GetArgVector("good_withs"));
    dto.statuses = ParseEnumVector<models::AnimalStatus>(request.GetArgVector("statuses"));
    dto.age_gte = ParseOptionalInt(request, "age_gte");
    dto.age_lte = ParseOptionalInt(request, "age_lte");

    return dto;
}

}  // namespace

AnimalListHandler::AnimalListHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      animal_service_(component_context.FindComponent<components::AnimalServiceComponent>()) {}

userver::formats::json::Value AnimalListHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& /*request_json*/,
    userver::server::request::RequestContext& context
) const {
    const auto filters = ParseFiltersFromRequest(request);
    int page = 1;
    if (request.HasArg("page")) {
        try {
            page = std::stoi(std::string(request.GetArg("page")));
        } catch (const std::exception&) {
            utils::ErrorResponse(utils::error_code::kValidationError, "page must be a positive integer")
                .ThrowClientError();
        }
        if (page < 1) {
            utils::ErrorResponse(utils::error_code::kValidationError, "page must be a positive integer")
                .ThrowClientError();
        }
    }
    std::optional<int64_t> user_id = std::nullopt;
    if (const int64_t* user_id_value = context.GetDataOptional<int64_t>("user_id")) {
        user_id = *user_id_value;
    }
    const auto result = animal_service_.get_service().FindByFilters(filters, page, user_id);
    return userver::formats::json::ValueBuilder{result}.ExtractValue();
}

}  // namespace pawspective::handlers
