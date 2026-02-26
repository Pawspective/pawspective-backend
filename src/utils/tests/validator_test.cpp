#include "utils/validator.hpp"
#include <gtest/gtest.h>
#include <userver/formats/json/serialize.hpp>

class ValidatorTest : public ::testing::Test {
protected:
    pawspective::utils::Validator validator_;
};

TEST_F(ValidatorTest, ValidInputDoesNotThrow) {
    EXPECT_NO_THROW({
        validator_.Field("email", "test@example.com")
            .NotEmpty()
            .Matches(userver::utils::regex(R"(^\S+@\S+\.\S+$)"), "Bad email")
            .Field("age", "25")
            .NotEmpty()
            .ThrowIfInvalid();
    });
}

TEST_F(ValidatorTest, IsOneOfSuccess) {
    EXPECT_NO_THROW({ validator_.Field("role", "admin").IsOneOf({"admin", "user", "guest"}).ThrowIfInvalid(); });
}

TEST_F(ValidatorTest, NotEmptyFailure) {
    try {
        validator_.Field("username", "").NotEmpty().ThrowIfInvalid();
        FAIL() << "Validator should have thrown ValidationException";
    } catch (const pawspective::utils::ValidationException& e) {
        const auto& errors = e.GetErrors();
        ASSERT_EQ(errors.size(), 1);
        EXPECT_EQ(errors[0].field_name, "username");
        EXPECT_EQ(errors[0].error_message, "must not be empty");
    }
}

TEST_F(ValidatorTest, MultipleFieldsFailure) {
    try {
        validator_.Field("email", "invalid-email")
            .Matches(userver::utils::regex(R"(^\S+@\S+\.\S+$)"), "Invalid format")
            .Field("password", "123")
            .MinLength(8)
            .ThrowIfInvalid();
        FAIL() << "Validator should have thrown for multiple errors";
    } catch (const pawspective::utils::ValidationException& e) {
        const auto& errors = e.GetErrors();
        ASSERT_EQ(errors.size(), 2);
        EXPECT_EQ(errors[0].field_name, "email");
        EXPECT_EQ(errors[0].error_message, "Invalid format");
        EXPECT_EQ(errors[1].field_name, "password");
        EXPECT_EQ(errors[1].error_message, "must be at least 8 characters long");
    }
}

TEST_F(ValidatorTest, ExceptionJsonStructure) {
    try {
        validator_.Field("status", "unknown").IsOneOf({"active", "pending"}).ThrowIfInvalid();
    } catch (const pawspective::utils::ValidationException& e) {
        auto json_res = e.GetExternalResponse();

        EXPECT_EQ(json_res["status"].As<std::string>(), "error");
        EXPECT_TRUE(json_res["details"].IsArray());
        EXPECT_EQ(json_res["details"][0]["field"].As<std::string>(), "status");
    }
}