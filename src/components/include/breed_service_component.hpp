#pragma once

#include <userver/components/component_base.hpp>
#include "breed_service.hpp"

namespace pawspective::components {
class BreedServiceComponent : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "breed-service";
    explicit BreedServiceComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    [[nodiscard]] const services::BreedService& get_service() const;

private:
    const services::BreedService service_;
};
}  // namespace pawspective::components
