#include "s3_component.hpp"
#include <chrono>
#include <userver/clients/http/component.hpp>
#include <userver/s3api/authenticators/access_key.hpp>
#include <userver/s3api/clients/s3api.hpp>
#include <userver/s3api/models/s3api_connection_type.hpp>
#include <userver/s3api/models/secret.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/secdist.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include <userver/yaml_config/schema.hpp>
#include "s3_secrets.hpp"

namespace pawspective::components {

namespace {

utils::S3UploadClient MakeClient(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
) {
    const auto& secdist = context.FindComponent<userver::components::Secdist>().Get();
    const auto& secrets = secdist.Get<models::S3Secrets>();
    auto& http_client = context.FindComponent<userver::components::HttpClient>().GetHttpClient();

    const auto bucket = config["bucket"].As<std::string>();
    const auto endpoint = config["endpoint"].As<std::string>();
    const auto public_url_base = config["public_url_base"].As<std::string>();
    const auto timeout_ms = config["timeout"].As<int>(5000);

    auto connection = userver::s3api::MakeS3Connection(
        http_client,
        userver::s3api::S3ConnectionType::kHttps,
        endpoint,
        userver::s3api::ConnectionCfg{std::chrono::milliseconds{timeout_ms}, 3}
    );

    auto auth = std::make_shared<
        userver::s3api::authenticators::AccessKey>(secrets.access_key, userver::s3api::Secret{secrets.secret_key});

    auto s3_client = userver::s3api::GetS3Client(std::move(connection), std::move(auth), bucket);

    return utils::S3UploadClient{std::move(s3_client), public_url_base};
}

}  // namespace

S3Component::S3Component(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : LoggableComponentBase(config, context), client_(MakeClient(config, context)) {}

utils::S3UploadClient& S3Component::GetClient() { return client_; }

userver::yaml_config::Schema S3Component::GetStaticConfigSchema() {
    return userver::yaml_config::MergeSchemas<LoggableComponentBase>(R"(
        type: object
        description: S3-compatible object storage configuration
        additionalProperties: false
        properties:
            bucket:
                type: string
                description: Storage bucket name
            endpoint:
                type: string
                description: S3-compatible endpoint hostname (e.g. storage.yandexcloud.net)
            public_url_base:
                type: string
                description: Base public URL for uploaded files (e.g. https://bucket.storage.yandexcloud.net)
            timeout:
                type: integer
                description: HTTP request timeout in milliseconds
    )");
}

}  // namespace pawspective::components
