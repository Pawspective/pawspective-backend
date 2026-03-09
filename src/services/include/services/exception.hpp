#pragma once

#include <stdexcept>

namespace pawspective::services {
class UserAlreadyExistsException : public std::runtime_error {
public:
    explicit UserAlreadyExistsException();
};

class UserNotFoundException : public std::runtime_error {
public:
    explicit UserNotFoundException();
};

class InvalidCredentialsException : public std::runtime_error {
public:
    explicit InvalidCredentialsException();
};

class OrganizationNotFoundException : public std::runtime_error {
public:
    explicit OrganizationNotFoundException();
};

}  // namespace pawspective::services