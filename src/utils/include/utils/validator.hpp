#include <string>
#include <userver/utils/regex.hpp>
#include <vector>
#include "utils/exception.hpp"
namespace pawspective::utils {

class Validator {
public:
    explicit Validator() = default;

    Validator& Field(std::string name, std::string value);

    Validator& NotBlank();

    Validator& MinLength(std::size_t min);

    Validator& MaxLength(std::size_t max);

    Validator& Matches(const userver::utils::regex& re, std::string msg);

    Validator& IsOneOf(const std::vector<std::string>& allowed);

    void ThrowIfInvalid();

private:
    void AddError(std::string msg);

    std::string current_field_;
    std::string current_value_;
    std::vector<ValidationException::FieldError> errors_;
};

}  // namespace pawspective::utils