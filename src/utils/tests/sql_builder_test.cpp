#include "utils/sql_builder.hpp"

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
    EXPECT_EQ(query_clause.parameters.Size(), 4);
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
    EXPECT_EQ(query_clause.parameters.Size(), 3);
}

UTEST(SqlBuilder, EmptyFilterProducesNoClauses) {
    const QueryFilter filter;

    const auto query_clause = BuildQueryClause(filter, MakeWhitelist());

    EXPECT_TRUE(query_clause.query.empty());
    EXPECT_EQ(query_clause.parameters.Size(), 0);
}

UTEST(SqlBuilder, ThrowsOnUnknownField) {
    QueryFilter filter;
    filter.conditions.push_back(Condition::Eq("unknown_filter", 1));

    EXPECT_THROW(static_cast<void>(BuildQueryClause(filter, MakeWhitelist())), std::invalid_argument);

    QueryFilter sort_filter;
    sort_filter.sort_specs.push_back(SortSpec{"unknown_sort", SortOrder::kAsc});

    EXPECT_THROW(static_cast<void>(BuildQueryClause(sort_filter, MakeWhitelist())), std::invalid_argument);
}

}  // namespace pawspective::utils::sql::tests
