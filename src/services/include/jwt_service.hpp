#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <userver/crypto/signers.hpp>
#include <userver/crypto/verifiers.hpp>

namespace pawspective::services {

/**
 * @brief Decoded JWT payload returned after successful validation.
 *
 */
struct TokenPayload {
    std::string user_id;  ///< Subject indentifier extracted from the token.
    bool is_refresh_token{false
    };  ///< Indicates whether the token is a refresh token or an access token.
};

/**
 * @brief Stateless JWT service responsible for token creation and validation.
 *
 * This service:
 *  - generates signed access and refresh tokens
 *  - validates token signature
 *  - validates expiration (`exp`) claim
 *  - verifies token type (access and refresh)
 *
 * This service does NOT:
 *  - manage sessions
 *  - perform token revocation checks
 *  - interact with storage
 *
 * Cryptographic signing and verification are performed using HS256.
 */
class JwtService final {
public:
    struct Config {
        std::string secret_key;  ///< Symmetric key used for HS256 signing.
        std::chrono::seconds access_ttl;   ///< Lifetime of access tokens.
        std::chrono::seconds refresh_ttl;  ///< Lifetime of refresh tokens.
    };

    /**
     * @brief Construct a new JwtService object with given configuration.
     *
     * @param config  Configuration parameters for the JWT service, including
     * secret key and token lifetimes.
     */
    explicit JwtService(const Config &config);

    /**
     * @brief Generates a JWT access token.
     *
     * The token contains subject (`sub`) claim with the user ID, issued at
     * (`iat`) and expiration (`exp`) claims, and a custom `ref` claim set to
     * false. The token is signed using HS256 and is valid for the duration
     * specified in the configuration. *
     * @param user_id Identifier of the authenticated user.
     * @return std::string signed JWT access token.
     */
    [[nodiscard]] std::string generate_access_token(std::string_view user_id
    ) const;

    /**
     * @brief Generates a signed JWT refresh token.
     *
     * The token contains subject (`sub`), expiration (`exp`),
     * issuer (`iss`) and token type claims.
     *
     * @param user_id Identifier of the authenticated user.
     * @return Signed JWT refresh token.
     */
    [[nodiscard]] std::string generate_refresh_token(std::string_view user_id
    ) const;

    /**
     * @brief Validates an access token.
     *
     * Performs:
     *  - signature verification
     *  - expiration validation
     *  - token type check (must be access)
     *
     * @param token JWT string.
     * @return Decoded payload if valid; std::nullopt otherwise.
     */
    [[nodiscard]] std::optional<TokenPayload> validate_access_token(
        std::string_view token
    ) const;

    /**
     * @brief Validates a refresh token.
     *
     * Performs:
     *  - signature verification
     *  - expiration validation
     *  - token type check (must be refresh)
     *
     * @param token JWT string.
     * @return Decoded payload if valid; std::nullopt otherwise.
     */
    [[nodiscard]] std::optional<TokenPayload> validate_refresh_token(
        std::string_view token
    ) const;

private:
    /**
     * @brief Internal helper for token creation.
     */
    [[nodiscard]] std::string create_token(
        std::string_view user_id,
        bool is_refresh_token,
        std::chrono::seconds ttl
    ) const;

    /**
     * @brief Internal helper for signature and expiration validation.
     *
     * Does not check token type.
     */
    [[nodiscard]] std::optional<TokenPayload> validate_token(
        std::string_view token
    ) const;

    const std::chrono::seconds access_ttl_;
    const std::chrono::seconds refresh_ttl_;

    userver::crypto::SignerHs256 signer_;
    userver::crypto::VerifierHs256 verifier_;
};

}  // namespace pawspective::services
