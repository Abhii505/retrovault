#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

class VaultException : public std::exception {
protected:
    std::string message;
public:
    explicit VaultException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class InvalidROMException : public VaultException {
public:
    explicit InvalidROMException(const std::string& msg)
        : VaultException("[InvalidROMException] " + msg) {}
};

class InsufficientStorageException : public VaultException {
public:
    explicit InsufficientStorageException(const std::string& msg)
        : VaultException("[InsufficientStorageException] " + msg) {}
};

class CorruptedChecksumException : public VaultException {
public:
    explicit CorruptedChecksumException(const std::string& msg)
        : VaultException("[CorruptedChecksumException] " + msg) {}
};

#endif