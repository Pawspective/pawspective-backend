#pragma once

#include <stdexcept>

namespace pawspective::services {
class UserAlreadyExistsException : public std::runtime_error {
public:
    explicit UserAlreadyExistsException();
};
}  // namespace pawspective::services