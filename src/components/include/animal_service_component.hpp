#pragma once

#include <userver/components/component_base.hpp>
#include "animal_service.hpp"

namespace pawspective::components {
class AnimalServiceComponent : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "animal-service";
    explicit AnimalServiceComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    [[nodiscard]] const services::AnimalService& get_service() const;

private:
    const services::AnimalService service_;
};
}  // namespace pawspective::components
