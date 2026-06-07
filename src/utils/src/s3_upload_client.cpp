#include "utils/s3_upload_client.hpp"
#include <fmt/format.h>
#include <userver/formats/json/value_builder.hpp>
#include <userver/logging/log.hpp>
#include <userver/s3api/clients/s3api.hpp>
#include <userver/testsuite/testpoint.hpp>

namespace pawspective::utils {

S3UploadClient::S3UploadClient(userver::s3api::ClientPtr client, std::string public_url_base)
    : client_(std::move(client)), public_url_base_(std::move(public_url_base)) {
    while (!public_url_base_.empty() && public_url_base_.back() == '/') {
        public_url_base_.pop_back();
    }
}

std::string S3UploadClient::UploadFile(std::string_view key, std::string data, std::string_view content_type) {
    try {
        client_->PutObject(key, std::move(data), std::nullopt, content_type);
    } catch (const std::exception& e) {
        LOG_ERROR() << "S3 upload failed for key=" << key << ": " << e.what();
        throw;
    }
    return fmt::format("{}/{}", public_url_base_, key);
}

void S3UploadClient::DeleteFile(std::string_view filename) {
    const auto key = fmt::format("photos/{}", filename);
    TESTPOINT("s3-delete-file", [&key] {
        userver::formats::json::ValueBuilder builder;
        builder["key"] = key;
        return builder.ExtractValue();
    }());

    try {
        client_->DeleteObject(key);
    } catch (const std::exception& e) {
        LOG_ERROR() << "S3 delete failed for filename=" << filename << ": " << e.what();
        throw;
    }
}

}  // namespace pawspective::utils
