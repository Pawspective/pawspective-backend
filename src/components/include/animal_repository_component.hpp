#pragma once
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/components/loggable_component_base.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include "animal_repository.hpp"

namespace pawspective::components {
class AnimalRepositoryComponent final : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "animal-repository";
    explicit AnimalRepositoryComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    [[nodiscard]] const repositories::AnimalRepository& get_repository() const;

private:
    const repositories::AnimalRepository repository_;
};
}  // namespace pawspective::components