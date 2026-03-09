#pragma once

#include <userver/components/component_base.hpp>
#include "organization_service.hpp"

namespace pawspective::components {
class OrganizationServiceComponent : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "organization-service";
    explicit OrganizationServiceComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    [[nodiscard]] const services::OrganizationService& get_service() const;

private:
    const services::OrganizationService service_;
};
}  // namespace pawspective::components
