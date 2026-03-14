#include "services/exception.hpp"

#include <stdexcept>

namespace pawspective::services {
UserAlreadyExistsException::UserAlreadyExistsException() : std::runtime_error("User already exists") {}

UserNotFoundException::UserNotFoundException() : std::runtime_error("User not found") {}

InvalidCredentialsException::InvalidCredentialsException() : std::runtime_error("Invalid credentials") {}

OrganizationNotFoundException::OrganizationNotFoundException() : std::runtime_error("Organization not found") {}

CityNotFoundException::CityNotFoundException() : std::runtime_error("City not found") {}

AnimalNotFoundException::AnimalNotFoundException() : std::runtime_error("Animal not found") {}

BreedNotFoundException::BreedNotFoundException() : std::runtime_error("Breed not found") {}

ForbiddenException::ForbiddenException() : std::runtime_error("Forbidden") {}

}  // namespace pawspective::services
