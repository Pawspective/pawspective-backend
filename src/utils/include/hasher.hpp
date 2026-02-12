#pragma once
#include <string>
#include <userver/crypto/exception.hpp>

namespace pawspective::utils::crypto {

class HashError : public userver::crypto::CryptoException {
    using userver::crypto::CryptoException::CryptoException;
};

[[nodiscard]] std::string generate_hash(const std::string &data);

[[nodiscard]] bool
verify_hash(const std::string &data, const std::string &hash);
}  // namespace pawspective::utils::crypto
