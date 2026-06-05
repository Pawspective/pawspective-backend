#pragma once

#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
#include "s3_component.hpp"

namespace pawspective::handlers {

class UploadPhotoHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-upload-photo";

    UploadPhotoHandler(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context,
        bool is_monitor = false
    );

    std::string HandleRequest(
        userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context
    ) const override;

private:
    components::S3Component& s3_;
};

}  // namespace pawspective::handlers
