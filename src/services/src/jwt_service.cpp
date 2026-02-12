#include "jwt_service.hpp"
#include <string>
#include <string_view>
#include <userver/crypto/base64.hpp>
#include <userver/formats/json.hpp>
#include <userver/utils/datetime.hpp>

namespace pawspective::services {

JwtService::JwtService(const Config &config)
    : access_ttl_(config.access_ttl),
      refresh_ttl_(config.refresh_ttl),
      signer_(config.secret_key),
      verifier_(config.secret_key) {
}

std::string JwtService::generate_access_token(const std::string_view user_id
) const {
    return create_token(user_id, false, access_ttl_);
}

std::string JwtService::generate_refresh_token(const std::string_view user_id
) const {
    return create_token(user_id, true, refresh_ttl_);
}

std::string JwtService::create_token(
    const std::string_view user_id,
    bool is_refresh_token,
    std::chrono::seconds ttl
) const {
    auto now = userver::utils::datetime::Now();
    auto expiry = now + ttl;
    userver::formats::json::ValueBuilder payload;
    payload["sub"] = user_id;
    payload["iat"] =
        std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch())
            .count();
    payload["exp"] = std::chrono::duration_cast<std::chrono::seconds>(
                         expiry.time_since_epoch()
    )
                         .count();
    payload["ref"] = is_refresh_token;
    payload["iss"] = "auth-service";

    const std::string payload_json =
        userver::formats::json::ToString(payload.ExtractValue());
    const std::string payload_b64 =
        userver::crypto::base64::Base64UrlEncode(payload_json);

    const std::string header_json = R"({"alg":"HS256","typ":"JWT"})";
    const std::string header_b64 =
        userver::crypto::base64::Base64UrlEncode(header_json);

    const std::string to_sign = header_b64 + "." + payload_b64;

    const std::string signature = signer_.Sign({to_sign});
    return to_sign + "." + userver::crypto::base64::Base64UrlEncode(signature);
}

std::optional<TokenPayload> JwtService::validate_token(
    const std::string_view token
) const {
    const auto first_dot = token.find('.');
    const auto last_dot = token.rfind('.');

    if (first_dot == std::string::npos || last_dot == first_dot) {
        return std::nullopt;
    }

    const std::string_view signed_area = token.substr(0, last_dot);
    const std::string_view signature_b64 = token.substr(last_dot + 1);

    try {
        const std::string signature =
            userver::crypto::base64::Base64UrlDecode(signature_b64);
        verifier_.Verify({signed_area}, signature);
    } catch (...) {
        return std::nullopt;
    }

    try {
        const std::string_view payload_b64 = signed_area.substr(first_dot + 1);

        auto payload_json = userver::formats::json::FromString(
            userver::crypto::base64::Base64UrlDecode(payload_b64)
        );

        auto exp = payload_json["exp"].As<std::int64_t>();
        if (exp < userver::utils::datetime::Timestamp()) {
            return std::nullopt;
        }

        auto header_json = signed_area.substr(0, first_dot);
        if (userver::formats::json::FromString(
                userver::crypto::base64::Base64UrlDecode(header_json)
            )["alg"]
                .As<std::string>() != "HS256") {
            return std::nullopt;
        }

        if (payload_json["iss"].As<std::string>() != "auth-service") {
            return std::nullopt;
        }

        return TokenPayload{
            payload_json["sub"].As<std::string>(),
            payload_json["ref"].As<bool>()
        };
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<TokenPayload> JwtService::validate_access_token(
    const std::string_view token
) const {
    auto payload = validate_token(token);
    if (payload && !payload->is_refresh_token) {
        return payload;
    }
    return std::nullopt;
}

std::optional<TokenPayload> JwtService::validate_refresh_token(
    const std::string_view token
) const {
    auto payload = validate_token(token);
    if (payload && payload->is_refresh_token) {
        return payload;
    }
    return std::nullopt;
}

}  // namespace pawspective::services
