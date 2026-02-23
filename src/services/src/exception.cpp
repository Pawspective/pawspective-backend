#include "services/exception.hpp"

#include <stdexcept>

namespace pawspective::services {
UserAlreadyExistsException::UserAlreadyExistsException() : std::runtime_error("User already exists") {}

UserNotFoundException::UserNotFoundException() : std::runtime_error("User not found") {}

InvalidCredentialsException::InvalidCredentialsException() : std::runtime_error("Invalid credentials") {}

}  // namespace pawspective::services
