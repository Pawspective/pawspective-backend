#include "models/breed.hpp"

namespace pawspective::models {

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
