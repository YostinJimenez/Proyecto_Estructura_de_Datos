#include "BancoException.h"

BancoException::BancoException(const string& msg) : mensaje(msg) {}

const char* BancoException::what() const noexcept {
    return mensaje.c_str();
}