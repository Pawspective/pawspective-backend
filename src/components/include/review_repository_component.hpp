#pragma once
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/components/loggable_component_base.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include "review_repository.hpp"

namespace pawspective::components {
class ReviewRepositoryComponent final : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "review-repository";
    explicit ReviewRepositoryComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    [[nodiscard]] const repositories::ReviewRepository& get_repository() const;

private:
    const repositories::ReviewRepository repository_;
};
}  // namespace pawspective::components
