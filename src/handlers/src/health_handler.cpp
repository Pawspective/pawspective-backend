#include <health_handler.hpp>
#include <string>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>

namespace pawspective::handlers {
std::string HealthCheck::HandleRequest(
    userver::server::http::HttpRequest & /*reqest*/,
    userver::server::request::RequestContext & /*context*/
) const {
    return "OK";
}

}  // namespace handlers::health