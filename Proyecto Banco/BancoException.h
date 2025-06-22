#ifndef BANCOEXCEPTION_H
#define BANCOEXCEPTION_H
#include <string>
#include <exception>
using namespace std;

class BancoException : public exception {
    string mensaje;
public:
    BancoException(const string& msg);
    const char* what() const noexcept override;
};

#endif