/**
 * @brief Implementa la clase de excepción personalizada 
 * BancoException para manejar errores del sistema bancario.
 * @param msg Mensaje descriptivo del error que se almacenará 
 * en el objeto.
 * Inicializa el mensaje de error y proporciona el método what() 
 * para devolverlo como una cadena C-style,
 * permitiendo su uso en la gestión de excepciones dentro de 
 * las operaciones bancarias.
 */
#include "BancoException.h"

BancoException::BancoException(const string& msg) : mensaje(msg) {}

const char* BancoException::what() const noexcept {
    return mensaje.c_str();
}