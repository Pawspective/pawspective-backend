#pragma once

#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/server/handlers/exceptions.hpp>

namespace pawspective::utils {

/**
 * @brief userver message builder that marks the external body as already
 * formatted, preventing the framework from wrapping it in its own error
 * envelope (e.g. LegacyJsonErrorBuilder's {"code":…,"message":…} wrapper).
 */
struct FormattedErrorBody {
    static constexpr bool kIsExternalBodyFormatted = true;

    explicit FormattedErrorBody(std::string body) : body_(std::move(body)) {}

    [[nodiscard]] std::string GetExternalBody() const { return body_; }

private:
    std::string body_;
};

/**
 * @brief Builder for constructing standardized error responses
 */
class ErrorResponse {
public:
    explicit ErrorResponse(const std::string& message);

    /**
     * @brief Get the error response as JSON value with the given HTTP status code
     */
    [[nodiscard]] userver::formats::json::Value GetJson(int http_status) const;

    /**
     * @brief Get the error response as serialized JSON string with the given HTTP status code
     */
    [[nodiscard]] std::string GetString(int http_status) const;

    /**
     * @brief Throw ClientError (HTTP 400) with this error response
     */
    [[noreturn]] void ThrowClientError() const;

    /**
     * @brief Throw Unauthorized (HTTP 401) with this error response
     */
    [[noreturn]] void ThrowUnauthorized() const;

    /**
     * @brief Throw ConflictError (HTTP 409) with this error response
     */
    [[noreturn]] void ThrowConflict() const;

    /**
     * @brief Throw ResourceNotFound (HTTP 404) with this error response
     */
    [[noreturn]] void ThrowNotFound() const;

private:
    std::string message_;
};

}  // namespace pawspective::utils
