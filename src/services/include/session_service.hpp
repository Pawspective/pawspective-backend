#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "jwt_service.hpp"

namespace pawspective::services {

/**
 * @brief Represents a bundle of session tokens.
 */
struct SessionBundle {
    std::string access_token;
    std::string refresh_token;
};

/**
 * @brief Interface for session management service.
 *
 * This service is responsible for:
 *  - creating new sessions (generating access and refresh tokens)
 *  - validating sessions based on refresh tokens
 *  - revoking sessions by invalidating refresh tokens
 *
 * The actual implementation of this interface would typically interact with
 * a storage to manage refresh tokens and support revocation.
 * The access token is stateless and is returned to the client without being
 * stored in the service.
 */
class SessionService {
public:
    /**
     * @brief Creates a new session for the given user ID.
     *
     * The refresh token is recorded in the service storage for validation and
     * revocation checks.
     *
     * @param user_id  Unique identifier of the user for whom the session is
     * created.
     * @return A SessionBundle containing the generated access and refresh
     * tokens.
     */

    [[nodiscard]] virtual SessionBundle create_session(std::int64_t user_id) const = 0;

    /**
     * @brief Validates a session based on the provided refresh token.
     *
     * Performs:
     *  - signature verification
     *  - expiration validation
     *  - existence check in storage (to ensure token is not revoked)
     *
     * @param refresh_token JWT refresh token.
     * @return Decoded payload if valid; std::nullopt otherwise.
     */
    [[nodiscard]] virtual std::optional<TokenPayload> validate_session(std::string_view refresh_token) const = 0;

    /**
     * @brief Revokes a session associated with the given refresh token.
     *
     * Removes the refresh token from storage, effectively invalidating it.
     *
     * @param refresh_token Refresh token to revoke.
     */
    virtual void revoke_session(std::string_view refresh_token) const = 0;

    /**
     * @brief Revokes all expired sessions from storage.
     *
     */
    virtual std::size_t revoke_expired_sessions() const = 0;

    virtual ~SessionService() = default;
    SessionService() = default;
    SessionService(const SessionService&) = delete;
    SessionService& operator=(const SessionService&) = delete;
    SessionService(SessionService&&) = delete;
    SessionService& operator=(SessionService&&) = delete;
};

}  // namespace pawspective::services