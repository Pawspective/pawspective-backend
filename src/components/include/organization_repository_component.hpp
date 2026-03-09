#pragma once
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/components/loggable_component_base.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include "organization_repository.hpp"

namespace pawspective::components {
class OrganizationRepositoryComponent final : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "organization-repository";
    explicit OrganizationRepositoryComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    [[nodiscard]] const repositories::OrganizationRepository& get_repository() const;

private:
    const repositories::OrganizationRepository repository_;
};
}  // namespace pawspective::components