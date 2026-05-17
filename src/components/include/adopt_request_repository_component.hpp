#pragma once

#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/components/loggable_component_base.hpp>

#include "adopt_request_repository.hpp"

namespace pawspective::components {

class AdoptRequestRepositoryComponent final : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "adopt-request-repository";

    explicit AdoptRequestRepositoryComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    [[nodiscard]] const repositories::AdoptRequestRepository& get_repository() const;

private:
    const repositories::AdoptRequestRepository repository_;
};

}  // namespace pawspective::components
