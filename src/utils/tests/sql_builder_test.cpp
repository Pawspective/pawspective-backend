#include "utils/sql_builder.hpp"

#include <cstring>
#include <stdexcept>

#include <gtest/gtest.h>
#include <userver/utest/utest.hpp>

namespace pawspective::utils::sql::tests {

namespace {

QueryWhitelist MakeWhitelist() {
    return QueryWhitelist{
        .filter_fields =
            {
                {"name", "o.name"},
                {"city_id", "o.city_id"},
                {"rating", "o.rating"},
            },
        .sort_fields =
            {
                {"name", "o.name"},
                {"created_at", "o.created_at"},
            },
    };
}

void ExpectParameterStoreEquals(
    const userver::storages::postgres::ParameterStore& actual,
    const userver::storages::postgres::ParameterStore& expected
) {
    const auto& actual_data = actual.GetInternalData();
    const auto& expected_data = expected.GetInternalData();

    ASSERT_EQ(actual_data.Size(), expected_data.Size());

    const auto* actual_types = actual_data.ParamTypesBuffer();
    const auto* expected_types = expected_data.ParamTypesBuffer();
    const auto* actual_lengths = actual_data.ParamLengthsBuffer();
    const auto* expected_lengths = expected_data.ParamLengthsBuffer();
    const auto* actual_formats = actual_data.ParamFormatsBuffer();
    const auto* expected_formats = expected_data.ParamFormatsBuffer();
    const auto* actual_values = actual_data.ParamBuffers();
    const auto* expected_values = expected_data.ParamBuffers();

    for (size_t i = 0; i < actual_data.Size(); ++i) {
        SCOPED_TRACE(::testing::Message() << "Parameter index: " << i);

        EXPECT_EQ(actual_types[i], expected_types[i]);
        EXPECT_EQ(actual_lengths[i], expected_lengths[i]);
        EXPECT_EQ(actual_formats[i], expected_formats[i]);

        if (actual_values[i] == nullptr || expected_values[i] == nullptr) {
            EXPECT_EQ(actual_values[i], expected_values[i]);
            continue;
        }

        if (actual_lengths[i] > 0) {
            EXPECT_EQ(std::memcmp(actual_values[i], expected_values[i], static_cast<size_t>(actual_lengths[i])), 0);
        }
    }
}

}  // namespace

UTEST(SqlBuilder, BuildsExpectedSqlFragments) {
    QueryFilter filter;
    filter.conditions.push_back(Condition::Eq("city_id", 10));
    filter.conditions.push_back(Condition::Ilike("name", "cats"));
    filter.sort_specs.push_back(SortSpec{"created_at", SortOrder::kDesc});
    filter.page_spec.limit = 25;
    filter.page_spec.offset = 50;

    const auto query_clause = BuildQueryClause(filter, MakeWhitelist());

    EXPECT_EQ(
        query_clause.query,
        " WHERE o.city_id = $1"
        " AND o.name ILIKE '%' || $2 || '%' ESCAPE '\\'"
        " ORDER BY o.created_at DESC"
        " LIMIT $3"
        " OFFSET $4"
    );

    userver::storages::postgres::ParameterStore expected_parameters;
    expected_parameters.PushBack(10);
    expected_parameters.PushBack(std::string{"cats"});
    expected_parameters.PushBack(25);
    expected_parameters.PushBack(50);

    ExpectParameterStoreEquals(query_clause.parameters, expected_parameters);
}

UTEST(SqlBuilder, UsesMonotonicPlaceholderOrder) {
    QueryFilter filter;
    filter.conditions.push_back(Condition::Ge("rating", 3));
    filter.conditions.push_back(Condition::Any("city_id", std::vector<int>{1, 2, 3}));
    filter.sort_specs.push_back(SortSpec{"name", SortOrder::kAsc});
    filter.page_spec.limit = 10;

    const auto query_clause = BuildQueryClause(filter, MakeWhitelist());

    EXPECT_EQ(
        query_clause.query,
        " WHERE o.rating > $1"
        " AND o.city_id = ANY($2)"
        " ORDER BY o.name ASC"
        " LIMIT $3"
    );

    userver::storages::postgres::ParameterStore expected_parameters;
    expected_parameters.PushBack(3);
    expected_parameters.PushBack(std::vector<int>{1, 2, 3});
    expected_parameters.PushBack(10);

    ExpectParameterStoreEquals(query_clause.parameters, expected_parameters);
}

UTEST(SqlBuilder, EmptyFilterProducesNoClauses) {
    QueryFilter filter;

    const auto query_clause = BuildQueryClause(filter, MakeWhitelist());

    EXPECT_TRUE(query_clause.query.empty());

    userver::storages::postgres::ParameterStore expected_parameters;
    ExpectParameterStoreEquals(query_clause.parameters, expected_parameters);
}

UTEST(SqlBuilder, ThrowsOnUnknownField) {
    QueryFilter filter;
    filter.conditions.push_back(Condition::Eq("unknown_filter", 1));

    EXPECT_THROW(static_cast<void>(BuildQueryClause(filter, MakeWhitelist())), std::invalid_argument);

    QueryFilter sort_filter;
    sort_filter.sort_specs.push_back(SortSpec{"unknown_sort", SortOrder::kAsc});

    EXPECT_THROW(static_cast<void>(BuildQueryClause(sort_filter, MakeWhitelist())), std::invalid_argument);
}

UTEST(SqlBuilder, EscapesSpecialCharactersInIlike) {
    QueryFilter filter;
    filter.conditions.push_back(Condition::Ilike("name", R"(a%b_c\d)"));

    const auto query_clause = BuildQueryClause(filter, MakeWhitelist());

    EXPECT_EQ(query_clause.query, " WHERE o.name ILIKE '%' || $1 || '%' ESCAPE '\\'");

    userver::storages::postgres::ParameterStore expected_parameters;
    expected_parameters.PushBack(std::string{R"(a\%b\_c\\d)"});

    ExpectParameterStoreEquals(query_clause.parameters, expected_parameters);
}

UTEST(SqlBuilder, HandlesEmptyVectorInAny) {
    QueryFilter filter;
    filter.conditions.push_back(Condition::Any("city_id", std::vector<int>{}));

    const auto query_clause = BuildQueryClause(filter, MakeWhitelist());

    EXPECT_EQ(query_clause.query, " WHERE o.city_id = ANY($1)");

    userver::storages::postgres::ParameterStore expected_parameters;
    expected_parameters.PushBack(std::vector<int>{});

    ExpectParameterStoreEquals(query_clause.parameters, expected_parameters);
}

UTEST(SqlBuilder, BuildsOnlyOrderBy) {
    QueryFilter filter;
    filter.sort_specs.push_back(SortSpec{"name", SortOrder::kDesc});

    const auto query_clause = BuildQueryClause(filter, MakeWhitelist());

    EXPECT_EQ(query_clause.query, " ORDER BY o.name DESC");

    userver::storages::postgres::ParameterStore expected_parameters;
    ExpectParameterStoreEquals(query_clause.parameters, expected_parameters);
}

UTEST(SqlBuilder, BuildsOnlyLimit) {
    QueryFilter filter;
    filter.page_spec.limit = 7;

    const auto query_clause = BuildQueryClause(filter, MakeWhitelist());

    EXPECT_EQ(query_clause.query, " LIMIT $1");

    userver::storages::postgres::ParameterStore expected_parameters;
    expected_parameters.PushBack(7);
    ExpectParameterStoreEquals(query_clause.parameters, expected_parameters);
}

UTEST(SqlBuilder, BuildsOnlyOffset) {
    QueryFilter filter;
    filter.page_spec.offset = 12;

    const auto query_clause = BuildQueryClause(filter, MakeWhitelist());

    EXPECT_EQ(query_clause.query, " OFFSET $1");

    userver::storages::postgres::ParameterStore expected_parameters;
    expected_parameters.PushBack(12);
    ExpectParameterStoreEquals(query_clause.parameters, expected_parameters);
}

}  // namespace pawspective::utils::sql::tests
