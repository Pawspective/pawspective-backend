#pragma once

#include <userver/components/component_base.hpp>
#include "review_service.hpp"

namespace pawspective::components {
class ReviewServiceComponent : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "review-service";
    explicit ReviewServiceComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    [[nodiscard]] const services::ReviewService& get_service() const;

private:
    const services::ReviewService service_;
};
}  // namespace pawspective::components
