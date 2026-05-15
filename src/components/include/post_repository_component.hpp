#pragma once
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/components/loggable_component_base.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include "post_repository.hpp"

namespace pawspective::components {
class PostRepositoryComponent final : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "post-repository";
    explicit PostRepositoryComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    [[nodiscard]] const repositories::PostRepository& get_repository() const;

private:
    const repositories::PostRepository repository_;
};
}  // namespace pawspective::components