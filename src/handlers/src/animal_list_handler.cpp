#include "animal_list_handler.hpp"

#include <algorithm>
#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>  // NOLINT
#include "animal_filter_dto.hpp"
#include "animal_service_component.hpp"

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

std::optional<std::vector<std::int64_t>> ParseInt64Vector(const std::vector<std::string>& args) {
    if (args.empty()) {
        return std::nullopt;
    }
    std::vector<std::int64_t> result;
    result.reserve(args.size());
    std::transform(args.begin(), args.end(), std::back_inserter(result), [](const auto& s) { return std::stoll(s); });
    return result;
}

std::optional<int> ParseOptionalInt(const userver::server::http::HttpRequest& request, std::string_view name) {
    if (!request.HasArg(name)) {
        return std::nullopt;
    }
    return std::stoi(std::string(request.GetArg(name)));
}

dto::AnimalFilterDTO ParseFiltersFromRequest(const userver::server::http::HttpRequest& request) {
    dto::AnimalFilterDTO dto;

    dto.breeds = ParseInt64Vector(request.GetArgVector("breeds"));
    dto.city_ids = ParseInt64Vector(request.GetArgVector("city_ids"));
    dto.animal_types = ParseEnumVector<models::AnimalType>(request.GetArgVector("animal_types"));
    dto.sizes = ParseEnumVector<models::AnimalSize>(request.GetArgVector("sizes"));
    dto.genders = ParseEnumVector<models::AnimalGender>(request.GetArgVector("genders"));
    dto.care_levels = ParseEnumVector<models::CareLevel>(request.GetArgVector("care_levels"));
    dto.colors = ParseEnumVector<models::AnimalColor>(request.GetArgVector("colors"));
    dto.good_withs = ParseEnumVector<models::GoodWith>(request.GetArgVector("good_withs"));
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
    userver::server::request::RequestContext& /*context*/
) const {
    const auto filters = ParseFiltersFromRequest(request);
    const auto animals = animal_service_.get_service().FindByFilters(filters);
    return userver::formats::json::ValueBuilder{animals}.ExtractValue();
}

}  // namespace pawspective::handlers
