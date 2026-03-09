#pragma once
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/components/loggable_component_base.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include "city_repository.hpp"

namespace pawspective::components {
class CityRepositoryComponent final : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "city-repository";
    explicit CityRepositoryComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    [[nodiscard]] const repositories::CityRepository& get_repository() const;

private:
    const repositories::CityRepository repository_;
};
}  // namespace pawspective::components