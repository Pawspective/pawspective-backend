#include "../include/animal_repository.hpp"
#include "../../utils/include/utils/sql_builder.hpp"

#include <fmt/format.h>
#include <algorithm>
#include <boost/algorithm/string/join.hpp>
#include <userver/storages/postgres/io/array_types.hpp>
#include <userver/storages/postgres/io/enum_types.hpp>
#include <userver/storages/postgres/io/range_types.hpp>

namespace pawspective::repositories {

namespace {

const utils::sql::QueryWhitelist kFilterWhitelist{
    {{"breeds", "a.breed_id"},
     {"animalTypes", "b.animal_type::text"},
     {"sizes", "a.size::text"},
     {"genders", "a.gender::text"},
     {"careLevels", "a.care_level::text"},
     {"colors", "a.color::text"},
     {"goodWiths", "a.good_with::text"},
     {"age", "a.age"}},
    {{"id", "a.id"}}
};

template <typename T>
std::string EnumToLiteral(T value) {
    const auto& enumerators = userver::storages::postgres::io::CppToUserPg<T>::enumerators;
    const auto it = std::find_if(enumerators.begin(), enumerators.end(), [value](const auto& e) {
        return e.enumerator == value;
    });
    if (it == enumerators.end()) {
        throw std::runtime_error("Unknown enum value");
    }
    return std::string(it->literal);
}

template <typename T>
std::vector<std::string> EnumsToLiterals(const std::vector<T>& values) {
    std::vector<std::string> result;
    result.reserve(values.size());
    std::transform(values.begin(), values.end(), std::back_inserter(result), [](const auto& v) {
        return EnumToLiteral(v);
    });
    return result;
}

}  // namespace

AnimalRepository::AnimalRepository(userver::storages::postgres::ClusterPtr pg_cluster)
    : pg_cluster_(std::move(pg_cluster)) {}

[[nodiscard]] std::optional<models::Animal> AnimalRepository::GetById(int64_t id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, organization_id, name, breed_id, size, gender, "
        "care_level, good_with, color, age, description, status "
        "FROM animals WHERE id = $1",
        id
    );
    if (result.IsEmpty()) {
        return std::nullopt;
    }
    return result.AsSingleRow<models::Animal>(userver::storages::postgres::kRowTag);
}

[[nodiscard]] std::vector<models::Animal> AnimalRepository::GetByOrganizationId(int64_t org_id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, organization_id, name, breed_id, size, gender, "
        "care_level, good_with, color, age, description, status "
        "FROM animals WHERE organization_id = $1",
        org_id
    );
    return result.AsContainer<std::vector<models::Animal>>(userver::storages::postgres::kRowTag);
}

models::Animal AnimalRepository::Create(const models::Animal& animal) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO animals (organization_id, name, breed_id, size, gender, "
        "care_level, good_with, color, age, description, status) "
        "VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11) "
        "RETURNING id, organization_id, name, breed_id, size, gender, "
        "care_level, good_with, color, age, description, status",
        animal.organization_id,
        animal.name,
        animal.breed_id,
        animal.size,
        animal.gender,
        animal.care_level,
        animal.good_with,
        animal.color,
        animal.age,
        animal.description,
        animal.status
    );
    return result.AsSingleRow<models::Animal>(userver::storages::postgres::kRowTag);
}

std::optional<models::Animal> AnimalRepository::Update(const models::Animal& animal) const {
    userver::storages::postgres::ParameterStore parameters;
    std::vector<std::string> updates;
    parameters.PushBack(animal.id);
    auto add_field = [&](const std::string& field_name, auto value) {
        updates.push_back(fmt::format("{} = ${}", field_name, parameters.Size() + 1));
        parameters.PushBack(value);
    };
    if (!animal.name.empty()) {
        add_field("name", animal.name);
    }
    if (animal.breed_id != -1) {
        add_field("breed_id", animal.breed_id);
    }
    if (animal.size != models::AnimalSize::kUnspecified) {
        add_field("size", animal.size);
    }
    if (animal.gender != models::AnimalGender::kUnspecified) {
        add_field("gender", animal.gender);
    }
    if (animal.care_level != models::CareLevel::kUnspecified) {
        add_field("care_level", animal.care_level);
    }
    if (animal.status != models::AnimalStatus::kUnspecified) {
        add_field("status", animal.status);
    }
    if (animal.description.has_value()) {
        add_field("description", animal.description.value());
    }
    if (animal.good_with != models::GoodWith::kUnspecified) {
        add_field("good_with", animal.good_with);
    }
    if (animal.age != -1) {
        add_field("age", animal.age);
    }
    if (animal.color != models::AnimalColor::kUnspecified) {
        add_field("color", animal.color);
    }
    auto query = fmt::format(
        "UPDATE animals SET {} WHERE id = $1 "
        "RETURNING id, organization_id, name, breed_id, size, gender, "
        "care_level, good_with, color, age, description, status",
        fmt::join(updates, ", ")
    );
    auto result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, query, parameters);
    if (result.IsEmpty()) {
        return std::nullopt;
    }
    return result.AsSingleRow<models::Animal>(userver::storages::postgres::kRowTag);
}

models::AnimalFilters AnimalRepository::GetAvailableFilters() const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT "
        "ARRAY(SELECT DISTINCT breed_id FROM animals WHERE breed_id IS NOT NULL), "
        "ARRAY(SELECT DISTINCT b.animal_type FROM animals a JOIN breeds b ON a.breed_id = b.id), "
        "ARRAY(SELECT DISTINCT size FROM animals), "
        "ARRAY(SELECT DISTINCT gender FROM animals), "
        "ARRAY(SELECT DISTINCT care_level FROM animals), "
        "ARRAY(SELECT DISTINCT color FROM animals), "
        "ARRAY(SELECT DISTINCT good_with FROM animals), "
        "COALESCE(MIN(age), 0), "
        "COALESCE(MAX(age), 0) "
        "FROM animals"
    );

    auto row = result.AsSingleRow<std::tuple<
        std::vector<std::int64_t>,
        std::vector<models::AnimalType>,
        std::vector<models::AnimalSize>,
        std::vector<models::AnimalGender>,
        std::vector<models::CareLevel>,
        std::vector<models::AnimalColor>,
        std::vector<models::GoodWith>,
        int,
        int>>();

    return {
        std::get<0>(row),
        std::get<1>(row),
        std::get<2>(row),
        std::get<3>(row),
        std::get<4>(row),
        std::get<5>(row),
        std::get<6>(row),
        std::get<7>(row),
        std::get<8>(row)
    };
}

std::vector<models::Animal> AnimalRepository::FindByFilters(const models::AnimalFilters& filter) const {
    utils::sql::QueryFilter query_filter;

    auto add_any_enum = [&](const std::string& key, const auto& vec) {
        if (!vec.empty()) {
            query_filter.conditions.push_back(utils::sql::Condition::Any(key, EnumsToLiterals(vec)));
        }
    };

    if (!filter.breed_ids.empty()) {
        query_filter.conditions.push_back(utils::sql::Condition::Any("breeds", filter.breed_ids));
    }

    add_any_enum("animalTypes", filter.animal_types);
    add_any_enum("sizes", filter.sizes);
    add_any_enum("genders", filter.genders);
    add_any_enum("careLevels", filter.care_levels);
    add_any_enum("colors", filter.colors);
    add_any_enum("goodWiths", filter.good_withs);

    query_filter.conditions.push_back(utils::sql::Condition::Ge("age", filter.min_age));
    query_filter.conditions.push_back(utils::sql::Condition::Le("age", filter.max_age));

    auto [query_suffix, params] = utils::sql::BuildQueryClause(query_filter, kFilterWhitelist);

    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT a.id, a.organization_id, a.name, a.breed_id, a.size, a.gender, "
        "a.care_level, a.good_with, a.color, a.age, a.description, a.status "
        "FROM animals a "
        "LEFT JOIN breeds b ON a.breed_id = b.id " +
            query_suffix,
        params
    );

    return result.AsContainer<std::vector<models::Animal>>(userver::storages::postgres::kRowTag);
}

}  // namespace pawspective::repositories
