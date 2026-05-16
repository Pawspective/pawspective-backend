#pragma once

#include <cstdint>
#include <string>
#include <tuple>
#include "../../dto/include/adopt_request_dto.hpp"

namespace pawspective::models {

struct AdoptRequest {
    static dto::AdoptRequestDTO to_dto(
        const AdoptRequest& model,
        const std::string& email,
        const dto::AnimalDTO& animal
    );

    std::int64_t id{};
    std::int64_t animal_id{};
    std::int64_t user_id{};

    auto Introspect() { return std::tie(id, animal_id, user_id); }
};

}  // namespace pawspective::models
