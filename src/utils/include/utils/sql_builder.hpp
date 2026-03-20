#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <userver/storages/postgres/parameter_store.hpp>
#include <userver/utils/move_only_function.hpp>

namespace pawspective::utils::sql {

/**
 * @brief Callable that appends a single value to ParameterStore.
 */
using ValuePusher = userver::utils::move_only_function<void(userver::storages::postgres::ParameterStore&)>;

namespace detail {

template <typename T>
ValuePusher MakePusher(T&& value) {  // NOLINT(cppcoreguidelines-missing-std-forward)
    return [v = std::decay_t<T>(std::forward<T>(value))](auto& params) mutable { params.PushBack(std::move(v)); };
}

}  // namespace detail

enum class Op : std::uint8_t {
    kEqual,
    kNotEqual,
    kGreater,
    kLess,
    kGreaterOrEqual,
    kLessOrEqual,
    kIlike,
    kAny,
};

enum class SortOrder : std::uint8_t { kAsc, kDesc };

/**
 * @brief Single filtering condition.
 *
 * Field name is resolved via whitelist during SQL generation,
 * and value is always provided through the binder.
 */
struct Condition {
    /** @brief Logical field name from external filter input. */
    std::string column;
    /** @brief Comparison operator. */
    Op op;
    /** @brief Deferred binder that pushes condition value into ParameterStore. */
    ValuePusher binder;

    /**
     * @brief Builds ILIKE condition with LIKE-safe escaping.
     * @param column Logical field name.
     * @param value Input search text.
     * @return Condition with Op::kIlike and deferred parameter binding.
     */
    static Condition Ilike(std::string_view column, std::string_view value);

    /**
     * @brief Builds '=' condition.
     * @tparam T Value type accepted by ParameterStore::PushBack.
     * @param column Logical field name.
     * @param value Condition value.
     * @return Condition with Op::kEqual and deferred parameter binding.
     */
    template <typename T>
    static Condition Eq(std::string_view column, T&& value) {
        return Condition{std::string(column), Op::kEqual, detail::MakePusher(std::forward<T>(value))};
    }

    /**
     * @brief Builds '!=' condition.
     * @tparam T Value type accepted by ParameterStore::PushBack.
     * @param column Logical field name.
     * @param value Condition value.
     * @return Condition with Op::kNotEqual and deferred parameter binding.
     */
    template <typename T>
    static Condition Neq(std::string_view column, T&& value) {
        return Condition{std::string(column), Op::kNotEqual, detail::MakePusher(std::forward<T>(value))};
    }

    /**
     * @brief Builds '>' condition.
     * @tparam T Value type accepted by ParameterStore::PushBack.
     * @param column Logical field name.
     * @param value Condition value.
     * @return Condition with Op::kGreater and deferred parameter binding.
     */
    template <typename T>
    static Condition Ge(std::string_view column, T&& value) {
        return Condition{std::string(column), Op::kGreater, detail::MakePusher(std::forward<T>(value))};
    }

    /**
     * @brief Builds '<' condition.
     * @tparam T Value type accepted by ParameterStore::PushBack.
     * @param column Logical field name.
     * @param value Condition value.
     * @return Condition with Op::kLess and deferred parameter binding.
     */
    template <typename T>
    static Condition Le(std::string_view column, T&& value) {
        return Condition{std::string(column), Op::kLess, detail::MakePusher(std::forward<T>(value))};
    }

    /**
     * @brief Builds '>=' condition.
     * @tparam T Value type accepted by ParameterStore::PushBack.
     * @param column Logical field name.
     * @param value Condition value.
     * @return Condition with Op::kGreaterOrEqual and deferred parameter binding.
     */
    template <typename T>
    static Condition Geq(std::string_view column, T&& value) {
        return Condition{std::string(column), Op::kGreaterOrEqual, detail::MakePusher(std::forward<T>(value))};
    }

    /**
     * @brief Builds '<=' condition.
     * @tparam T Value type accepted by ParameterStore::PushBack.
     * @param column Logical field name.
     * @param value Condition value.
     * @return Condition with Op::kLessOrEqual and deferred parameter binding.
     */
    template <typename T>
    static Condition Leq(std::string_view column, T&& value) {
        return Condition{std::string(column), Op::kLessOrEqual, detail::MakePusher(std::forward<T>(value))};
    }

    /**
     * @brief Builds '= ANY(...)' condition from a copied vector.
     * @tparam T Element type accepted by ParameterStore::PushBack for vector<T>.
     * @param column Logical field name.
     * @param values Condition values copied into internal storage.
     * @return Condition with Op::kAny and deferred parameter binding.
     */
    template <typename T>
    static Condition Any(std::string_view column, const std::vector<T>& values) {
        return Condition{std::string(column), Op::kAny, detail::MakePusher(std::vector<T>(values))};
    }

    /**
     * @brief Builds '= ANY(...)' condition from a moved vector.
     * @tparam T Element type accepted by ParameterStore::PushBack for vector<T>.
     * @param column Logical field name.
     * @param values Condition values moved into internal storage.
     * @return Condition with Op::kAny and deferred parameter binding.
     */
    template <typename T>
    static Condition Any(std::string_view column, std::vector<T>&& values) {
        return Condition{std::string(column), Op::kAny, detail::MakePusher(std::move(values))};
    }
};

/**
 * @brief ORDER BY specification.
 */
struct SortSpec {
    /** @brief Logical field name resolved through sort whitelist. */
    std::string column;
    /** @brief Ordering direction. */
    SortOrder order;
};

/**
 * @brief Pagination specification.
 */
struct PageSpec {
    /** @brief LIMIT value. */
    std::optional<int> limit;
    /** @brief OFFSET value. */
    std::optional<int> offset;
};

/**
 * @brief Allowed fields map: external field name -> trusted SQL column/expression.
 */
using FieldWhitelist = std::unordered_map<std::string, std::string>;

/**
 * @brief Separate allow-lists for filtering and sorting.
 */
struct QueryWhitelist {
    /** @brief Allowed fields for WHERE conditions. */
    FieldWhitelist filter_fields;
    /** @brief Allowed fields for ORDER BY clauses. */
    FieldWhitelist sort_fields;
};

/**
 * @brief Aggregate filter input for query clause generation.
 */
struct QueryFilter {
    /** @brief AND-joined list of WHERE conditions. */
    std::vector<Condition> conditions;
    /** @brief ORDER BY elements. */
    std::vector<SortSpec> sort_specs;
    /** @brief LIMIT/OFFSET settings. */
    PageSpec page_spec;
};

/**
 * @brief Result of query clause construction.
 */
struct QueryClause {
    /** @brief SQL suffix with optional WHERE/ORDER BY/LIMIT/OFFSET parts. */
    std::string query;
    /** @brief Bound parameter values for placeholders in query. */
    userver::storages::postgres::ParameterStore parameters;
};

/**
 * @brief Builds SQL clauses from QueryFilter using strict whitelist resolution.
 *
 * Consumes deferred binders in filter conditions, so filter is non-const.
 * @param filter Input conditions, sorting and pagination.
 * @param whitelist Allowed mappings for filter and sort fields.
 * @return SQL clause string with corresponding ParameterStore values.
 * @throw std::invalid_argument If a filter or sort field is not present in whitelist.
 */
QueryClause BuildQueryClause(QueryFilter& filter, const QueryWhitelist& whitelist);

}  // namespace pawspective::utils::sql
