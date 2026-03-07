#include "models/animal_enums.hpp"

#include <userver/formats/json/value.hpp>

namespace pawspective::models {

userver::formats::json::Value
Serialize(AnimalSize value, userver::formats::serialize::To<userver::formats::json::Value>) {
    using ValueBuilder = userver::formats::json::ValueBuilder;
    switch (value) {
        case AnimalSize::kSmall:
            return ValueBuilder("small").ExtractValue();
        case AnimalSize::kMedium:
            return ValueBuilder("medium").ExtractValue();
        case AnimalSize::kLarge:
            return ValueBuilder("large").ExtractValue();
        default:
            throw userver::formats::json::Exception("Invalid AnimalSize enum value");
    }
}

AnimalSize Parse(const userver::formats::json::Value& value, userver::formats::parse::To<AnimalSize>) {
    const auto str = value.As<std::string>();
    if (str == "small") {
        return AnimalSize::kSmall;
    }
    if (str == "medium") {
        return AnimalSize::kMedium;
    }
    if (str == "large") {
        return AnimalSize::kLarge;
    }
    throw userver::formats::json::Exception("Invalid AnimalSize: " + str);
}

userver::formats::json::Value
Serialize(AnimalGender value, userver::formats::serialize::To<userver::formats::json::Value>) {
    using ValueBuilder = userver::formats::json::ValueBuilder;
    switch (value) {
        case AnimalGender::kMale:
            return ValueBuilder("male").ExtractValue();
        case AnimalGender::kFemale:
            return ValueBuilder("female").ExtractValue();
        case AnimalGender::kUnknown:
            return ValueBuilder("unknown").ExtractValue();
        default:
            throw userver::formats::json::Exception("Invalid AnimalGender enum value");
    }
}

AnimalGender Parse(const userver::formats::json::Value& value, userver::formats::parse::To<AnimalGender>) {
    const auto str = value.As<std::string>();
    if (str == "male") {
        return AnimalGender::kMale;
    }
    if (str == "female") {
        return AnimalGender::kFemale;
    }
    if (str == "unknown") {
        return AnimalGender::kUnknown;
    }
    throw userver::formats::json::Exception("Invalid AnimalGender: " + str);
}

userver::formats::json::Value
Serialize(CareLevel value, userver::formats::serialize::To<userver::formats::json::Value>) {
    using ValueBuilder = userver::formats::json::ValueBuilder;
    switch (value) {
        case CareLevel::kEasy:
            return ValueBuilder("easy").ExtractValue();
        case CareLevel::kModerate:
            return ValueBuilder("moderate").ExtractValue();
        case CareLevel::kDifficult:
            return ValueBuilder("difficult").ExtractValue();
        case CareLevel::kSpecialNeeds:
            return ValueBuilder("special_needs").ExtractValue();
        default:
            throw userver::formats::json::Exception("Invalid CareLevel enum value");
    }
}

CareLevel Parse(const userver::formats::json::Value& value, userver::formats::parse::To<CareLevel>) {
    const auto str = value.As<std::string>();
    if (str == "easy") {
        return CareLevel::kEasy;
    }
    if (str == "moderate") {
        return CareLevel::kModerate;
    }
    if (str == "difficult") {
        return CareLevel::kDifficult;
    }
    if (str == "special_needs") {
        return CareLevel::kSpecialNeeds;
    }
    throw userver::formats::json::Exception("Invalid CareLevel: " + str);
}

userver::formats::json::Value
Serialize(GoodWith value, userver::formats::serialize::To<userver::formats::json::Value>) {
    using ValueBuilder = userver::formats::json::ValueBuilder;
    switch (value) {
        case GoodWith::kDogs:
            return ValueBuilder("dogs").ExtractValue();
        case GoodWith::kCats:
            return ValueBuilder("cats").ExtractValue();
        case GoodWith::kChildren:
            return ValueBuilder("children").ExtractValue();
        case GoodWith::kElderly:
            return ValueBuilder("elderly").ExtractValue();
        default:
            throw userver::formats::json::Exception("Invalid GoodWith enum value");
    }
}

GoodWith Parse(const userver::formats::json::Value& value, userver::formats::parse::To<GoodWith>) {
    const auto str = value.As<std::string>();
    if (str == "dogs") {
        return GoodWith::kDogs;
    }
    if (str == "cats") {
        return GoodWith::kCats;
    }
    if (str == "children") {
        return GoodWith::kChildren;
    }
    if (str == "elderly") {
        return GoodWith::kElderly;
    }
    throw userver::formats::json::Exception("Invalid GoodWith: " + str);
}

userver::formats::json::Value
Serialize(AnimalColor value, userver::formats::serialize::To<userver::formats::json::Value>) {
    using ValueBuilder = userver::formats::json::ValueBuilder;
    switch (value) {
        case AnimalColor::kBlack:
            return ValueBuilder("black").ExtractValue();
        case AnimalColor::kWhite:
            return ValueBuilder("white").ExtractValue();
        case AnimalColor::kBrown:
            return ValueBuilder("brown").ExtractValue();
        case AnimalColor::kGrey:
            return ValueBuilder("grey").ExtractValue();
        case AnimalColor::kOrange:
            return ValueBuilder("orange").ExtractValue();
        case AnimalColor::kCream:
            return ValueBuilder("cream").ExtractValue();
        case AnimalColor::kTan:
            return ValueBuilder("tan").ExtractValue();
        case AnimalColor::kGolden:
            return ValueBuilder("golden").ExtractValue();
        case AnimalColor::kSpotted:
            return ValueBuilder("spotted").ExtractValue();
        case AnimalColor::kStriped:
            return ValueBuilder("striped").ExtractValue();
        case AnimalColor::kBrindle:
            return ValueBuilder("brindle").ExtractValue();
        case AnimalColor::kMixed:
            return ValueBuilder("mixed").ExtractValue();
        default:
            throw userver::formats::json::Exception("Invalid AnimalColor enum value");
    }
}

AnimalColor Parse(const userver::formats::json::Value& value, userver::formats::parse::To<AnimalColor>) {
    const auto str = value.As<std::string>();
    if (str == "black") {
        return AnimalColor::kBlack;
    }
    if (str == "white") {
        return AnimalColor::kWhite;
    }
    if (str == "brown") {
        return AnimalColor::kBrown;
    }
    if (str == "grey") {
        return AnimalColor::kGrey;
    }
    if (str == "orange") {
        return AnimalColor::kOrange;
    }
    if (str == "cream") {
        return AnimalColor::kCream;
    }
    if (str == "tan") {
        return AnimalColor::kTan;
    }
    if (str == "golden") {
        return AnimalColor::kGolden;
    }
    if (str == "spotted") {
        return AnimalColor::kSpotted;
    }
    if (str == "striped") {
        return AnimalColor::kStriped;
    }
    if (str == "brindle") {
        return AnimalColor::kBrindle;
    }
    if (str == "mixed") {
        return AnimalColor::kMixed;
    }
    throw userver::formats::json::Exception("Invalid AnimalColor: " + str);
}

userver::formats::json::Value
Serialize(AnimalStatus value, userver::formats::serialize::To<userver::formats::json::Value>) {
    using ValueBuilder = userver::formats::json::ValueBuilder;
    switch (value) {
        case AnimalStatus::kAvailable:
            return ValueBuilder("available").ExtractValue();
        case AnimalStatus::kAdopted:
            return ValueBuilder("adopted").ExtractValue();
        case AnimalStatus::kUnavailable:
            return ValueBuilder("unavailable").ExtractValue();
        default:
            throw userver::formats::json::Exception("Invalid AnimalStatus enum value");
    }
}

AnimalStatus Parse(const userver::formats::json::Value& value, userver::formats::parse::To<AnimalStatus>) {
    const auto str = value.As<std::string>();
    if (str == "available") {
        return AnimalStatus::kAvailable;
    }
    if (str == "adopted") {
        return AnimalStatus::kAdopted;
    }
    if (str == "unavailable") {
        return AnimalStatus::kUnavailable;
    }
    throw userver::formats::json::Exception("Invalid AnimalStatus: " + str);
}

userver::formats::json::Value
Serialize(AnimalType value, userver::formats::serialize::To<userver::formats::json::Value>) {
    using ValueBuilder = userver::formats::json::ValueBuilder;
    switch (value) {
        case AnimalType::kDog:
            return ValueBuilder("dog").ExtractValue();
        case AnimalType::kCat:
            return ValueBuilder("cat").ExtractValue();
        case AnimalType::kOther:
            return ValueBuilder("other").ExtractValue();
        default:
            throw userver::formats::json::Exception("Invalid AnimalType enum value");
    }
}

AnimalType Parse(const userver::formats::json::Value& value, userver::formats::parse::To<AnimalType>) {
    const auto str = value.As<std::string>();
    if (str == "dog") {
        return AnimalType::kDog;
    }
    if (str == "cat") {
        return AnimalType::kCat;
    }
    if (str == "other") {
        return AnimalType::kOther;
    }
    throw userver::formats::json::Exception("Invalid AnimalType: " + str);
}

}  // namespace pawspective::models
