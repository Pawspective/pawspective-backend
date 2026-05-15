#include "../include/animal_repository.hpp"
#include "../../utils/include/utils/sql_builder.hpp"

#include <fmt/format.h>
#include <algorithm>
#include <boost/algorithm/string/join.hpp>
#include <iterator>
#include <userver/storages/postgres/io/array_types.hpp>
#include <userver/storages/postgres/io/enum_types.hpp>
#include <userver/storages/postgres/io/range_types.hpp>

namespace pawspective::repositories {

namespace {

const utils::sql::QueryWhitelist kFilterWhitelist{
    {{"breeds", "a.breed_id"},
     {"cityIds", "o.city_id"},
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

template <typename T>
std::vector<T> ToEnumVector(const std::vector<std::string>& strings) {
    std::vector<T> enums;
    enums.reserve(strings.size());
    std::transform(strings.begin(), strings.end(), std::back_inserter(enums), [](const std::string& s) {
        return models::Parse(s, userver::formats::parse::To<T>{});
    });
    return enums;
}

}  // namespace

AnimalRepository::AnimalRepository(userver::storages::postgres::ClusterPtr pg_cluster)
    : pg_cluster_(std::move(pg_cluster)) {}

[[nodiscard]] std::optional<models::Animal> AnimalRepository::GetById(int64_t id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, organization_id, name, breed_id, user_id, size, gender, "
        "care_level, good_with, color, age, description, status "
        "FROM animals WHERE id = $1",
        id
    );
    if (result.IsEmpty()) {
        return std::nullopt;
    }
    return result.AsSingleRow<models::Animal>(userver::storages::postgres::kRowTag);
}

std::pair<std::vector<models::Animal>, std::int64_t> AnimalRepository::GetByOrganizationIdPaginated(
    int64_t org_id,
    int page,
    int limit
) const {
    if (page < 1 || limit <= 0) {
        throw std::invalid_argument("page must be >= 1 and limit must be > 0");
    }

    auto count_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT COUNT(*) FROM animals WHERE organization_id = $1",
        org_id
    );
    const std::int64_t total_count = count_result.AsSingleRow<std::int64_t>();

    auto data_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, organization_id, name, breed_id, user_id, size, gender, "
        "care_level, good_with, color, age, description, status "
        "FROM animals WHERE organization_id = $1 "
        "ORDER BY id ASC "
        "LIMIT $2 OFFSET $3",
        org_id,
        limit,
        static_cast<std::int64_t>(page - 1) * limit
    );
    auto animals = data_result.AsContainer<std::vector<models::Animal>>(userver::storages::postgres::kRowTag);

    return {std::move(animals), total_count};
}

models::Animal AnimalRepository::Create(const models::Animal& animal) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO animals (organization_id, name, breed_id, size, gender, "
        "care_level, good_with, color, age, description, status) "
        "VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11) "
        "RETURNING id, organization_id, name, breed_id, user_id, size, gender, "
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
        add_field("user_id", std::optional<std::int64_t>{std::nullopt});
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
        "RETURNING id, organization_id, name, breed_id, user_id, size, gender, "
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
        "ARRAY(SELECT DISTINCT id FROM breeds WHERE id IS NOT NULL), "
        "ARRAY(SELECT DISTINCT o.city_id FROM animals a JOIN organizations o ON a.organization_id = o.id), "
        "ARRAY(SELECT unnest(enum_range(NULL::animal_type))::text), "
        "ARRAY(SELECT unnest(enum_range(NULL::animal_size))::text), "
        "ARRAY(SELECT unnest(enum_range(NULL::animal_gender))::text), "
        "ARRAY(SELECT unnest(enum_range(NULL::care_level))::text), "
        "ARRAY(SELECT unnest(enum_range(NULL::animal_color))::text), "
        "ARRAY(SELECT unnest(enum_range(NULL::good_with))::text), "
        "COALESCE(MIN(age), 0), "
        "COALESCE(MAX(age), 100) "
        "FROM animals"
    );

    auto row = result.AsSingleRow<std::tuple<
        std::vector<std::int64_t>,
        std::vector<std::int64_t>,
        std::vector<std::string>,
        std::vector<std::string>,
        std::vector<std::string>,
        std::vector<std::string>,
        std::vector<std::string>,
        std::vector<std::string>,
        int,
        int>>(userver::storages::postgres::kRowTag);

    return {
        std::get<0>(row),
        std::get<1>(row),
        ToEnumVector<models::AnimalType>(std::get<2>(row)),
        ToEnumVector<models::AnimalSize>(std::get<3>(row)),
        ToEnumVector<models::AnimalGender>(std::get<4>(row)),
        ToEnumVector<models::CareLevel>(std::get<5>(row)),
        ToEnumVector<models::AnimalColor>(std::get<6>(row)),
        ToEnumVector<models::GoodWith>(std::get<7>(row)),
        std::get<8>(row),
        std::get<9>(row)
    };
}

std::pair<std::vector<models::Animal>, std::int64_t> AnimalRepository::FindByFiltersPaginated(
    const models::AnimalFilters& filter,
    int page,
    int limit
) const {
    if (page < 1 || limit <= 0) {
        throw std::invalid_argument("page must be >= 1 and limit must be > 0");
    }

    auto build_conditions = [&]() {
        utils::sql::QueryFilter qf;
        auto add_any_enum = [&](const std::string& key, const auto& vec) {
            if (!vec.empty()) {
                qf.conditions.push_back(utils::sql::Condition::Any(key, EnumsToLiterals(vec)));
            }
        };
        if (!filter.breed_ids.empty()) {
            qf.conditions.push_back(utils::sql::Condition::Any("breeds", filter.breed_ids));
        }
        if (!filter.city_ids.empty()) {
            qf.conditions.push_back(utils::sql::Condition::Any("cityIds", filter.city_ids));
        }
        add_any_enum("animalTypes", filter.animal_types);
        add_any_enum("sizes", filter.sizes);
        add_any_enum("genders", filter.genders);
        add_any_enum("careLevels", filter.care_levels);
        add_any_enum("colors", filter.colors);
        add_any_enum("goodWiths", filter.good_withs);
        qf.conditions.push_back(utils::sql::Condition::Ge("age", filter.min_age));
        qf.conditions.push_back(utils::sql::Condition::Le("age", filter.max_age));
        return qf;
    };

    auto count_filter = build_conditions();
    auto [count_clause, count_params] = utils::sql::BuildQueryClause(count_filter, kFilterWhitelist);
    auto count_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT COUNT(*) FROM animals a "
        "LEFT JOIN breeds b ON a.breed_id = b.id "
        "LEFT JOIN organizations o ON a.organization_id = o.id" +
            count_clause,
        count_params
    );
    const std::int64_t total_count = count_result.AsSingleRow<std::int64_t>();

    auto data_filter = build_conditions();
    data_filter.page_spec.limit = limit;
    data_filter.page_spec.offset = static_cast<int>(static_cast<std::int64_t>(page - 1) * limit);
    data_filter.sort_specs.push_back({"id", utils::sql::SortOrder::kAsc});
    auto [data_clause, data_params] = utils::sql::BuildQueryClause(data_filter, kFilterWhitelist);
    auto data_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT a.id, a.organization_id, a.name, a.breed_id, a.user_id, a.size, a.gender, "
        "a.care_level, a.good_with, a.color, a.age, a.description, a.status "
        "FROM animals a "
        "LEFT JOIN breeds b ON a.breed_id = b.id "
        "LEFT JOIN organizations o ON a.organization_id = o.id" +
            data_clause,
        data_params
    );
    auto animals = data_result.AsContainer<std::vector<models::Animal>>(userver::storages::postgres::kRowTag);

    return {std::move(animals), total_count};
}

std::optional<models::Animal> AnimalRepository::Adopt(std::int64_t animal_id, std::int64_t user_id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "UPDATE animals SET user_id = $2, status = 'adopted' WHERE id = $1 "
        "RETURNING id, organization_id, name, breed_id, user_id, size, gender, "
        "care_level, good_with, color, age, description, status",
        animal_id,
        user_id
    );
    if (result.IsEmpty()) {
        return std::nullopt;
    }
    return result.AsSingleRow<models::Animal>(userver::storages::postgres::kRowTag);
}

}  // namespace pawspective::repositories
