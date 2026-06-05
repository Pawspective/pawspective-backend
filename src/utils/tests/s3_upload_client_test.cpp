#include "utils/s3_upload_client.hpp"
#include <gmock/gmock.h>
#include <stdexcept>
#include <userver/s3api/utest/client_gmock.hpp>
#include <userver/utest/utest.hpp>

namespace pawspective::utils::tests {

UTEST(S3UploadClientTest, UploadFileReturnsPublicUrl) {
    auto mock = std::make_shared<userver::s3api::GMockClient>();
    EXPECT_CALL(
        *mock,
        PutObject(
            std::string_view{"images/cat.jpg"},
            ::testing::_,
            ::testing::_,
            std::string_view{"image/jpeg"},
            ::testing::_,
            ::testing::_
        )
    )
        .Times(1)
        .WillOnce(::testing::Return(std::string{"etag-value"}));

    S3UploadClient client{mock, "https://my-bucket.s3.us-east-1.amazonaws.com"};
    const auto url = client.UploadFile("images/cat.jpg", "file-data", "image/jpeg");

    EXPECT_EQ(url, "https://my-bucket.s3.us-east-1.amazonaws.com/images/cat.jpg");
}

UTEST(S3UploadClientTest, UploadFileThrowsOnS3Error) {
    auto mock = std::make_shared<userver::s3api::GMockClient>();
    EXPECT_CALL(*mock, PutObject(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(1)
        .WillOnce(::testing::Throw(std::runtime_error{"S3 error: Access Denied"}));

    S3UploadClient client{mock, "https://my-bucket.s3.us-east-1.amazonaws.com"};
    EXPECT_THROW(client.UploadFile("images/cat.jpg", "file-data", "image/jpeg"), std::runtime_error);
}

UTEST(S3UploadClientTest, PublicUrlContainsKey) {
    auto mock = std::make_shared<userver::s3api::GMockClient>();
    EXPECT_CALL(*mock, PutObject(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(std::string{}));

    S3UploadClient client{mock, "https://bucket.s3.eu-west-1.amazonaws.com"};
    const auto url = client.UploadFile("orgs/42/logo.png", "data", "image/png");

    EXPECT_EQ(url, "https://bucket.s3.eu-west-1.amazonaws.com/orgs/42/logo.png");
}

}  // namespace pawspective::utils::tests
