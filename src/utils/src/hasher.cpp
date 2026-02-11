#include "hasher.hpp"
#include <argon2.h>
#include <string>
#include <userver/crypto/random.hpp>
#include <userver/logging/log.hpp>
#include <userver/utils/encoding/hex.hpp>
#include <vector>

namespace pawspective::utils::crypto {

namespace {
constexpr uint32_t kTimeCost = 3;
constexpr uint32_t kMemoryCost = 65536;  // 64 MB
constexpr uint32_t kParallelism = 1;
constexpr uint32_t kHashLength = 32;
constexpr uint32_t kSaltLength = 16;

}  // namespace

std::string generate_hash(const std::string &data) {
    auto salt = userver::crypto::GenerateRandomBlock(kSaltLength);

    size_t encoded_len = argon2_encodedlen(
        kTimeCost, kMemoryCost, kParallelism, kSaltLength, kHashLength,
        Argon2_id
    );
    std::vector<char> buffer(encoded_len);

    int result = argon2id_hash_encoded(
        kTimeCost, kMemoryCost, kParallelism, data.data(), data.size(),
        salt.data(), kSaltLength, kHashLength, buffer.data(), buffer.size()
    );

    if (result != ARGON2_OK) {
        LOG_ERROR() << "Argon2 error: " << argon2_error_message(result);
        throw HashError("Hash generation failed");
    }

    return {buffer.data()};
}

bool verify_hash(const std::string &data, const std::string &hash) {
    if (hash.empty()) {
        return false;
    }

    // argon2id_verify сам извлекает соль и параметры из строки hash
    int result = argon2id_verify(hash.data(), data.data(), data.size());

    if (result == ARGON2_OK) {
        return true;
    }
    if (result == ARGON2_VERIFY_MISMATCH) {
        return false;
    }

    LOG_WARNING() << "Argon2 verification failed: "
                  << argon2_error_message(result);
    return false;
}

}  // namespace pawspective::utils::crypto