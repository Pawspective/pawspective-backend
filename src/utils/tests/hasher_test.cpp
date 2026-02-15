#include "utils/hasher.hpp"
#include <gtest/gtest.h>
#include <string>
#include <userver/utest/utest.hpp>

namespace pawspective::utils::crypto::tests {
using ::pawspective::utils::crypto::GenerateHash;
using ::pawspective::utils::crypto::VerifyHash;

const std::string password = "my_secure_password_123!";

UTEST(CryptoUtils, HashCreateAndVerify) {
    const std::string hash = GenerateHash(password);

    EXPECT_FALSE(hash.empty());
    EXPECT_NE(password, hash);

    EXPECT_TRUE(VerifyHash(password, hash));
    EXPECT_FALSE(VerifyHash(std::string("wrong_password"), hash));
}

UTEST(CryptoUtils, DifferentSaltsForSamePassword) {
    const std::string hash1 = GenerateHash(password);
    const std::string hash2 = GenerateHash(password);

    EXPECT_NE(hash1, hash2);
    EXPECT_TRUE(VerifyHash(password, hash1));
    EXPECT_TRUE(VerifyHash(password, hash2));
}

UTEST(CryptoUtils, EmptyInputs) {
    const auto empty_hash = GenerateHash("");
    EXPECT_FALSE(empty_hash.empty());
    EXPECT_TRUE(VerifyHash("", empty_hash));

    EXPECT_FALSE(VerifyHash("", ""));
}

UTEST(CryptoUtils, InvalidHashFormat) {
    EXPECT_FALSE(VerifyHash(password, "not_a_hash"));
    EXPECT_FALSE(VerifyHash(password, "$2b$12$short"));
}

UTEST(CryptoUtils, LongPassword) {
    const std::string long_password(100, 'a');
    const auto hash = GenerateHash(long_password);

    EXPECT_TRUE(VerifyHash(long_password, hash));
}

}  // namespace pawspective::utils::crypto::tests
