#pragma once

#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/server/handlers/exceptions.hpp>

namespace pawspective::utils {

/**
 * @brief Builder for constructing standardized error responses
 */
class ErrorResponse {
public:
    explicit ErrorResponse(const std::string_view& code, const std::string& message);

    /**
     * @brief Get the error response as JSON value
     */
    [[nodiscard]] userver::formats::json::Value GetJson() const;

    /**
     * @brief Get the error response as string for ExternalBody
     */
    [[nodiscard]] std::string GetString() const;

    /**
     * @brief Throw ClientError with this error response
     */
    [[noreturn]] void ThrowClientError() const;

    /**
     * @brief Throw Unauthorized with this error response
     */
    [[noreturn]] void ThrowUnauthorized() const;

    /**
     * @brief Throw ConflictError with this error response
     */
    [[noreturn]] void ThrowConflict() const;

private:
    std::string code_;
    std::string message_;
};

/**
 * @brief Common error codes
 */
namespace error_code {
inline constexpr std::string_view kInvalidJsonFormat = "INVALID_JSON_FORMAT";
inline constexpr std::string_view kMissingField = "MISSING_FIELD";
inline constexpr std::string_view kValidationError = "VALIDATION_ERROR";
inline constexpr std::string_view kInvalidCredentials = "INVALID_CREDENTIALS";
inline constexpr std::string_view kUserNotFound = "USER_NOT_FOUND";
inline constexpr std::string_view kUserAlreadyExists = "USER_ALREADY_EXISTS";
inline constexpr std::string_view kInvalidRefreshToken = "INVALID_REFRESH_TOKEN";
}  // namespace error_code

}  // namespace pawspective::utils
