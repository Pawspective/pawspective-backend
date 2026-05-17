#pragma once

#include <userver/components/component_base.hpp>
#include "adopt_request_service.hpp"

namespace pawspective::components {

class AdoptRequestServiceComponent : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "adopt-request-service";

    explicit AdoptRequestServiceComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    [[nodiscard]] const services::AdoptRequestService& get_service() const;

private:
    const services::AdoptRequestService service_;
};

}  // namespace pawspective::components
