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

class CityNotFoundException : public std::runtime_error {
public:
    explicit CityNotFoundException();
};

class AnimalNotFoundException : public std::runtime_error {
public:
    explicit AnimalNotFoundException();
};

class BreedNotFoundException : public std::runtime_error {
public:
    explicit BreedNotFoundException();
};

class ForbiddenException : public std::runtime_error {
public:
    explicit ForbiddenException();
};

class AnimalNotAvailableException : public std::runtime_error {
public:
    explicit AnimalNotAvailableException();
};

class ReviewNotFoundException : public std::runtime_error {
public:
    explicit ReviewNotFoundException();
};

class ReviewAlreadyExistsException : public std::runtime_error {
public:
    explicit ReviewAlreadyExistsException();
};

class InvalidToken : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

}  // namespace pawspective::services