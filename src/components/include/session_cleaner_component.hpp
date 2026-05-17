#pragma once

#include <userver/storages/postgres/dist_lock_component_base.hpp>
#include "pg_session_component.hpp"

namespace pawspective::components {
class SessionCleanerComponent final : public userver::storages::postgres::DistLockComponentBase {
public:
    constexpr static std::string_view kName = "session-cleaner";

    SessionCleanerComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    ~SessionCleanerComponent() override;

protected:
    void DoWork() override;
    void DoWorkTestsuite() override;

private:
    PgSessionComponent& session_component_;
};
}  // namespace pawspective::components
