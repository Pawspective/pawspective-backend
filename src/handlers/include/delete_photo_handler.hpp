#pragma once

#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
#include "s3_component.hpp"

namespace pawspective::handlers {

class DeletePhotoHandler final : public userver::server::handlers::HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-delete-photo";

    DeletePhotoHandler(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context,
        bool is_monitor = false
    );

    userver::formats::json::Value HandleRequestJsonThrow(
        const userver::server::http::HttpRequest& request,
        const userver::formats::json::Value& request_body,
        userver::server::request::RequestContext& context
    ) const override;

private:
    components::S3Component& s3_;
};

}  // namespace pawspective::handlers
