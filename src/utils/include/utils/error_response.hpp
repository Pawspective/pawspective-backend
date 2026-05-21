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
    explicit ErrorResponse(const std::string_view& code, const std::string& message);

    /**
     * @brief Get the error response as JSON value
     */
    [[nodiscard]] userver::formats::json::Value GetJson() const;

    /**
     * @brief Get the error response as serialized JSON string
     */
    [[nodiscard]] std::string GetString() const;

    /**
     * @brief Throw ClientError (HTTP 400) with this error response
     */
    [[noreturn]] void ThrowClientError() const;

    /**
     * @brief Throw Unauthorized (HTTP 401) with this error response
     */
    [[noreturn]] void ThrowUnauthorized() const;

    /**
     * @brief Throw Forbidden (HTTP 403) with this error response
     */
    [[noreturn]] void ThrowForbidden() const;

    /**
     * @brief Throw ConflictError (HTTP 409) with this error response
     */
    [[noreturn]] void ThrowConflict() const;

    /**
     * @brief Throw ResourceNotFound (HTTP 404) with this error response
     */
    [[noreturn]] void ThrowNotFound() const;

private:
    std::string code_;
    std::string message_;
};

/**
 * @brief Machine-readable error codes (UPPER_SNAKE_CASE) used in error responses
 */
namespace error_code {
inline constexpr std::string_view kForbidden = "FORBIDDEN";
inline constexpr std::string_view kInvalidJsonFormat = "INVALID_JSON_FORMAT";
inline constexpr std::string_view kMissingField = "MISSING_FIELD";
inline constexpr std::string_view kValidationError = "VALIDATION_ERROR";
inline constexpr std::string_view kInvalidCredentials = "INVALID_CREDENTIALS";
inline constexpr std::string_view kUserNotFound = "USER_NOT_FOUND";
inline constexpr std::string_view kUserAlreadyExists = "USER_ALREADY_EXISTS";
inline constexpr std::string_view kAccessTokenMissing = "ACCESS_TOKEN_MISSING";
inline constexpr std::string_view kAccessTokenInvalid = "ACCESS_TOKEN_INVALID";
inline constexpr std::string_view kAccessTokenExpired = "ACCESS_TOKEN_EXPIRED";
inline constexpr std::string_view kRefreshTokenInvalid = "REFRESH_TOKEN_INVALID";
inline constexpr std::string_view kOrganizationNotFound = "ORGANIZATION_NOT_FOUND";
inline constexpr std::string_view kCityNotFound = "CITY_NOT_FOUND";
inline constexpr std::string_view kBreedNotFound = "BREED_NOT_FOUND";
inline constexpr std::string_view kAnimalNotFound = "ANIMAL_NOT_FOUND";
inline constexpr std::string_view kPostNotFound = "POST_NOT_FOUND";
inline constexpr std::string_view kReviewAlreadyExists = "REVIEW_ALREADY_EXISTS";
}  // namespace error_code

}  // namespace pawspective::utils
