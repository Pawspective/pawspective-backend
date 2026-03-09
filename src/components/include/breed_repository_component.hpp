#pragma once
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/components/loggable_component_base.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include "../../repositories/include/breed_repository.hpp"

namespace pawspective::components {
class BreedRepositoryComponent final : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "breed-repository";
    explicit BreedRepositoryComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    [[nodiscard]] const repositories::BreedRepository& get_repository() const;

private:
    const repositories::BreedRepository repository_;
};
}  // namespace pawspective::components