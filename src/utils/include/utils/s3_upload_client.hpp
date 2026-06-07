#pragma once

#include <string>
#include <string_view>
#include <userver/s3api/clients/fwd.hpp>

namespace pawspective::utils {

class S3UploadClient {
public:
    S3UploadClient(userver::s3api::ClientPtr client, std::string public_url_base);

    std::string UploadFile(std::string_view key, std::string data, std::string_view content_type);
    void DeleteFile(std::string_view url);

private:
    userver::s3api::ClientPtr client_;
    std::string public_url_base_;
};

}  // namespace pawspective::utils
