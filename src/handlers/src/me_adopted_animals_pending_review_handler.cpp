#include "me_adopted_animals_pending_review_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>

namespace pawspective::handlers {

MeAdoptedAnimalsPendingReviewHandler::MeAdoptedAnimalsPendingReviewHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context,
    bool is_monitor
)
    : HttpHandlerJsonBase(config, component_context, is_monitor),
      animal_service_(component_context.FindComponent<components::AnimalServiceComponent>()) {}

userver::formats::json::Value MeAdoptedAnimalsPendingReviewHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& /*request*/,
    const userver::formats::json::Value& /*request_json*/,
    userver::server::request::RequestContext& context
) const {
    const std::int64_t user_id = context.GetData<std::int64_t>("user_id");
    const auto result = animal_service_.get_service().GetAdoptedAnimalsWithoutReviewsByUserId(user_id);
    return userver::formats::json::ValueBuilder{result}.ExtractValue();
}

}  // namespace pawspective::handlers