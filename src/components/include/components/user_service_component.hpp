#pragma once

#include <userver/components/component_base.hpp>
#include "services/user_service.hpp"

namespace pawspective::components {
class UserServiceComponent : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "user-service";
    explicit UserServiceComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    [[nodiscard]] const services::UserService& get_service() const;

private:
    const services::UserService service_;
};
}  // namespace pawspective::components
