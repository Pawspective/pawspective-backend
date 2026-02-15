#pragma once
#include <string>
#include <userver/crypto/exception.hpp>

namespace pawspective::utils::crypto {

class HashError : public userver::crypto::CryptoException {
    using userver::crypto::CryptoException::CryptoException;
};

[[nodiscard]] std::string GenerateHash(const std::string& data);

[[nodiscard]] bool VerifyHash(const std::string& data, const std::string& hash);
}  // namespace pawspective::utils::crypto
