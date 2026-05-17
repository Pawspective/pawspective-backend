#pragma once

#include <userver/components/loggable_component_base.hpp>

#include "../../services/include/post_service.hpp"

namespace pawspective::components {

class PostServiceComponent final : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "post-service";

    explicit PostServiceComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    [[nodiscard]] const services::PostService& get_service() const;

private:
    const services::PostService service_;
};

}  // namespace pawspective::components