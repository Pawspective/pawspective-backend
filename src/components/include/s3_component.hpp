#pragma once

#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/components/loggable_component_base.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include "utils/s3_upload_client.hpp"

namespace pawspective::components {

class S3Component final : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "s3-service";

    explicit S3Component(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    [[nodiscard]] utils::S3UploadClient& GetClient();

    static userver::yaml_config::Schema GetStaticConfigSchema();

private:
    utils::S3UploadClient client_;
};

}  // namespace pawspective::components
