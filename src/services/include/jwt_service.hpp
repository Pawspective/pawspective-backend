#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <userver/crypto/signers.hpp>
#include <userver/crypto/verifiers.hpp>

namespace services {
struct TokenPayload {
    std::string user_id;
    bool is_refresh_token{false};
};

class JwtService final {
public:
    struct Config {
        std::string secret_key;
        std::chrono::seconds access_ttl;
        std::chrono::seconds refresh_ttl;
    };

    explicit JwtService(const Config &config);
    [[nodiscard]] std::string generate_access_token(std::string_view user_id
    ) const;
    [[nodiscard]] std::string generate_refresh_token(std::string_view user_id
    ) const;

    [[nodiscard]] std::optional<TokenPayload> validate_token(
        std::string_view token
    ) const;

private:
    [[nodiscard]] std::string create_token(
        std::string_view user_id,
        bool is_refresh_token,
        std::chrono::seconds ttl
    ) const;

    const std::chrono::seconds access_ttl_;
    const std::chrono::seconds refresh_ttl_;

    userver::crypto::SignerHs256 signer_;
    userver::crypto::VerifierHs256 verifier_;
};

}  // namespace services
