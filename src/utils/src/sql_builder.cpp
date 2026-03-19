#include "utils/sql_builder.hpp"

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
        case Op::kIlike:
            return "ILIKE";
        case Op::kAny:
            return "ANY";
    }
}

std::string EscapeForLike(std::string_view input) {
    std::string escaped;
    escaped.reserve(input.size());

    for (char c : input) {
        if (c == '%' || c == '_' || c == '\\') {
            escaped += '\\';
        }
        escaped += c;
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

Condition Condition::Ilike(std::string_view column, const std::string& value) {
    return Condition{std::string(column), Op::kIlike, detail::MakePusher(EscapeForLike(value))};
}

QueryClause BuildQueryClause(const QueryFilter& filter, const QueryWhitelist& whitelist) {
    std::string query;
    userver::storages::postgres::ParameterStore params;
    // WHERE part
    if (!filter.conditions.empty()) {
        query += " WHERE ";
        for (size_t i = 0; i < filter.conditions.size(); ++i) {
            auto& cond = filter.conditions[i];
            const auto& column = GetColumnName(whitelist.filter_fields, cond.column, "filters");
            switch (cond.op) {
                case Op::kIlike:
                    query += column + " ILIKE '%' || $" + std::to_string(params.Size() + 1) + " || '%' ESCAPE '\\'";
                    break;
                case Op::kAny:
                    query += column + " = ANY($" + std::to_string(params.Size() + 1) + ")";
                    break;
                default:
                    query += column + " " + OpToSql(cond.op) + " $" + std::to_string(params.Size() + 1);
            }
            cond.binder(params);
            if (i < filter.conditions.size() - 1) {
                query += " AND ";
            }
        }
    }
    // ORDER BY part
    if (!filter.sort_specs.empty()) {
        query += " ORDER BY ";
        for (size_t i = 0; i < filter.sort_specs.size(); ++i) {
            const auto& sort_spec = filter.sort_specs[i];
            const auto& column = GetColumnName(whitelist.sort_fields, sort_spec.column, "sorting");
            query += column + " " + (sort_spec.order == SortOrder::kAsc ? "ASC" : "DESC");
            if (i < filter.sort_specs.size() - 1) {
                query += ", ";
            }
        }
    }
    // LIMIT and OFFSET part
    const auto& page_spec = filter.page_spec;
    if (page_spec.limit.has_value()) {
        query += " LIMIT $" + std::to_string(params.Size() + 1);
        params.PushBack(page_spec.limit.value());
    }
    if (page_spec.offset.has_value()) {
        query += " OFFSET $" + std::to_string(params.Size() + 1);
        params.PushBack(page_spec.offset.value());
    }

    return QueryClause{std::move(query), std::move(params)};
}

}  // namespace pawspective::utils::sql
