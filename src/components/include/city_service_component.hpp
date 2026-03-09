#pragma once

#include <userver/components/component_base.hpp>
#include "city_service.hpp"

namespace pawspective::components {
class CityServiceComponent : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "city-service";
    explicit CityServiceComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    [[nodiscard]] const services::CityService& get_service() const;

private:
    const services::CityService service_;
};
}  // namespace pawspective::components
