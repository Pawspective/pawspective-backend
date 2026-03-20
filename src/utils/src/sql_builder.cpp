#include "utils/sql_builder.hpp"

#include <fmt/compile.h>
#include <fmt/format.h>
#include <algorithm>

#include <iterator>
#include <stdexcept>

namespace pawspective::utils::sql {

namespace {
std::string OpToSql(Op op) {
    switch (op) {
        case Op::kEqual:
            return "=";
        case Op::kNotEqual:
            return "!=";
        case Op::kGreater:
            return ">";
        case Op::kLess:
            return "<";
        case Op::kGreaterOrEqual:
            return ">=";
        case Op::kLessOrEqual:
            return "<=";
        default:
            throw std::logic_error("Unhandled operator in OpToSql");
    }
}

std::string EscapeForLike(std::string_view input) {
    const auto escape_count = static_cast<size_t>(std::count_if(input.begin(), input.end(), [](char c) {
        return c == '%' || c == '_' || c == '\\';
    }));

    if (escape_count == 0) {
        return std::string(input);
    }

    std::string escaped;
    escaped.reserve(input.size() + escape_count);

    for (char c : input) {
        if (c == '%' || c == '_' || c == '\\') {
            escaped.push_back('\\');
        }
        escaped.push_back(c);
    }
    return escaped;
}

const std::string& GetColumnName(
    const FieldWhitelist& whitelist,
    std::string_view field_name,
    std::string_view section
) {
    const auto it = whitelist.find(std::string(field_name));
    if (it == whitelist.end()) {
        throw std::invalid_argument("Field is not allowed in " + std::string(section) + ": " + std::string(field_name));
    }
    return it->second;
}
}  // namespace

Condition Condition::Ilike(std::string_view column, const std::string_view value) {
    return Condition{std::string(column), Op::kIlike, detail::MakePusher(EscapeForLike(value))};
}

QueryClause BuildQueryClause(QueryFilter& filter, const QueryWhitelist& whitelist) {
    fmt::memory_buffer query;
    query.reserve(64 + filter.conditions.size() * 48 + filter.sort_specs.size() * 24);
    userver::storages::postgres::ParameterStore params;
    // WHERE part
    if (!filter.conditions.empty()) {
        fmt::format_to(std::back_inserter(query), FMT_COMPILE(" WHERE "));
        for (size_t i = 0; i < filter.conditions.size(); ++i) {
            auto& cond = filter.conditions[i];
            const auto& column = GetColumnName(whitelist.filter_fields, cond.column, "filters");
            const auto next_placeholder = params.Size() + 1;
            switch (cond.op) {
                case Op::kIlike:
                    fmt::format_to(
                        std::back_inserter(query),
                        FMT_COMPILE("{} ILIKE '%' || ${} || '%' ESCAPE '\\'"),
                        column,
                        next_placeholder
                    );
                    break;
                case Op::kAny:
                    fmt::format_to(std::back_inserter(query), FMT_COMPILE("{} = ANY(${})"), column, next_placeholder);
                    break;
                default:
                    fmt::format_to(
                        std::back_inserter(query),
                        FMT_COMPILE("{} {} ${}"),
                        column,
                        OpToSql(cond.op),
                        next_placeholder
                    );
            }
            cond.binder(params);
            if (i < filter.conditions.size() - 1) {
                fmt::format_to(std::back_inserter(query), FMT_COMPILE(" AND "));
            }
        }
    }
    // ORDER BY part
    if (!filter.sort_specs.empty()) {
        fmt::format_to(std::back_inserter(query), FMT_COMPILE(" ORDER BY "));
        for (size_t i = 0; i < filter.sort_specs.size(); ++i) {
            const auto& sort_spec = filter.sort_specs[i];
            const auto& column = GetColumnName(whitelist.sort_fields, sort_spec.column, "sorting");
            fmt::format_to(
                std::back_inserter(query),
                FMT_COMPILE("{} {}"),
                column,
                sort_spec.order == SortOrder::kAsc ? "ASC" : "DESC"
            );
            if (i < filter.sort_specs.size() - 1) {
                fmt::format_to(std::back_inserter(query), FMT_COMPILE(", "));
            }
        }
    }
    // LIMIT and OFFSET part
    const auto& page_spec = filter.page_spec;
    if (page_spec.limit.has_value()) {
        fmt::format_to(std::back_inserter(query), FMT_COMPILE(" LIMIT ${}"), params.Size() + 1);
        params.PushBack(page_spec.limit.value());
    }
    if (page_spec.offset.has_value()) {
        fmt::format_to(std::back_inserter(query), FMT_COMPILE(" OFFSET ${}"), params.Size() + 1);
        params.PushBack(page_spec.offset.value());
    }

    return QueryClause{fmt::to_string(query), std::move(params)};
}

}  // namespace pawspective::utils::sql
