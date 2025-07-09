/**
 * @brief Define una clase de excepción personalizada para manejar errores específicos del sistema bancario.
 * @param msg Mensaje descriptivo del error ocurrido.
 * Hereda de la clase estándar exception y proporciona un método what() para devolver el mensaje de error,
 * permitiendo una gestión estructurada de excepciones en las operaciones bancarias.
 */
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