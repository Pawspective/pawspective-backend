#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <userver/utils/move_only_function.hpp>
#include <userver/storages/postgres/parameter_store.hpp>

namespace pawspective::utils::sql {

using ValuePusher = userver::utils::move_only_function<void(userver::storages::postgres::ParameterStore&) const>;

namespace detail {

template<typename T>
ValuePusher MakePusher(T&& value) {
    return [v = std::decay_t<T>(std::forward<T>(value))](auto& params) {
        params.PushBack(std::move(v));
    };
}



} // namespace detail


enum class Op {
    kEqual,
    kNotEqual,
    kGreater,
    kLess,
    kGreaterOrEqual,
    kLessOrEqual,
    kIlike,
    kAny,
};

enum class SortOrder {
    kAsc,
    kDesc
};

struct Condition {


    std::string column;
    Op op;
    ValuePusher binder;

    static Condition Ilike(std::string_view column, const std::string& value);

    template<typename T> 
    static Condition Eq(std::string_view column, T&& value) {
        return Condition{std::string(column), Op::kEqual, detail::MakePusher(std::forward<T>(value))};
    }

    template<typename T>
    static Condition Neq(std::string_view column, T&& value) {
        return Condition{std::string(column), Op::kNotEqual, detail::MakePusher(std::forward<T>(value))};
    }

    template<typename T>
    static Condition Ge(std::string_view column, T&& value) {
        return Condition{std::string(column), Op::kGreater, detail::MakePusher(std::forward<T>(value))};
    }

    template<typename T>
    static Condition Le(std::string_view column, T&& value) {
        return Condition{std::string(column), Op::kLess, detail::MakePusher(std::forward<T>(value))};
    }   

    template<typename T>
    static Condition Geq(std::string_view column, T&& value) {
        return Condition{std::string(column), Op::kGreaterOrEqual, detail::MakePusher(std::forward<T>(value))};
    }   

    template<typename T>
    static Condition Leq(std::string_view column, T&& value) {
        return Condition{std::string(column), Op::kLessOrEqual, detail::MakePusher(std::forward<T>(value))};
    }

    template<typename T>
    static Condition Any(std::string_view column, const std::vector<T>& values) {
        return Condition{std::string(column), Op::kAny, detail::MakePusher(std::move(std::vector<T>(values)))};
    }

    template<typename T>
    static Condition Any(std::string_view column, std::vector<T>&& values) {
        return Condition{std::string(column), Op::kAny, detail::MakePusher(std::move(values))};
    }
};

struct SortSpec {
    std::string column;
    SortOrder order;
};

struct PageSpec {
    std::optional<int> limit;
    std::optional<int> offset;
};

using FieldWhitelist = std::unordered_map<std::string, std::string>;

struct QueryWhitelist {
    FieldWhitelist filter_fields;
    FieldWhitelist sort_fields;
};

struct QueryFilter {
    std::vector<Condition> conditions;
    std::vector<SortSpec> sort_specs;
    PageSpec page_spec;
};

struct QueryClause {
    std::string query;
    userver::storages::postgres::ParameterStore parameters;
};


QueryClause BuildQueryClause(const QueryFilter& filter, const QueryWhitelist& whitelist);


} // namespace pawspective::utils::sql

