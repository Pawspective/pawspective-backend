#include "session_cleaner_component.hpp"

#include <userver/engine/sleep.hpp>
#include <userver/logging/log.hpp>
#include <userver/tracing/span.hpp>

namespace pawspective::components {
SessionCleanerComponent::SessionCleanerComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : userver::storages::postgres::DistLockComponentBase(config, context),
      session_component_(context.FindComponent<PgSessionComponent>()) {
    AutostartDistLock();
    LOG_INFO() << "SessionCleanerComponent initialized successfully";
}

void SessionCleanerComponent::DoWork() {
    LOG_INFO() << "SessionCleanerComponent started, lock acquired successfully";
    auto span = userver::tracing::Span::MakeRootSpan("session-cleaner-iteration");
    while (!IsCancelAdvised()) {
        try {
            const auto revoked_count = session_component_.get_service().revoke_expired_sessions();
            if (revoked_count > 0) {
                LOG_INFO() << "Revoked " << revoked_count << " expired sessions";
            } else {
                LOG_DEBUG() << "No expired sessions to revoke";
            }
        } catch (const std::exception& ex) {
            LOG_ERROR() << "Error while revoking expired sessions: " << ex.what();
        }
        if (userver::engine::current_task::ShouldCancel()) {
            break;
        }
        userver::engine::SleepFor(std::chrono::seconds(40));
    }
}

void SessionCleanerComponent::DoWorkTestsuite() { DoWork(); }

SessionCleanerComponent::~SessionCleanerComponent() {
    StopDistLock();
    LOG_INFO() << "SessionCleanerComponent is shutting down, releasing lock";
}

}  // namespace pawspective::components
