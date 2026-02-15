#include "services/exception.hpp"

#include <stdexcept>

namespace pawspective::services {
UserAlreadyExistsException::UserAlreadyExistsException() : std::runtime_error("User already exists") {}
}  // namespace pawspective::services
