#include "../include/hasher.hpp"
#include <userver/utest/utest.hpp>

namespace pawspective::utils::crypto::tests {
using ::pawspective::utils::crypto::generate_hash;
using ::pawspective::utils::crypto::verify_hash;

const std::string password = "my_secure_password_123!";

UTEST(CryptoUtils, HashCreateAndVerify) {
    const std::string hash = generate_hash(password);

    EXPECT_FALSE(hash.empty());
    EXPECT_NE(password, hash);

    EXPECT_TRUE(verify_hash(password, hash));
    EXPECT_FALSE(verify_hash(std::string("wrong_password"), hash));
}

UTEST(CryptoUtils, DifferentSaltsForSamePassword) {
    const std::string hash1 = generate_hash(password);
    const std::string hash2 = generate_hash(password);

    EXPECT_NE(hash1, hash2);
    EXPECT_TRUE(verify_hash(password, hash1));
    EXPECT_TRUE(verify_hash(password, hash2));
}

UTEST(CryptoUtils, EmptyInputs) {
    const auto empty_hash = generate_hash("");
    EXPECT_FALSE(empty_hash.empty());
    EXPECT_TRUE(verify_hash("", empty_hash));

    EXPECT_FALSE(verify_hash("", ""));
}

UTEST(CryptoUtils, InvalidHashFormat) {
    EXPECT_FALSE(verify_hash(password, "not_a_hash"));
    EXPECT_FALSE(verify_hash(password, "$2b$12$short"));
}

UTEST(CryptoUtils, LongPassword) {
    std::string long_password(100, 'a');
    const auto hash = generate_hash(long_password);

    EXPECT_TRUE(verify_hash(long_password, hash));
}

}  // namespace pawspective::utils::crypto::tests
