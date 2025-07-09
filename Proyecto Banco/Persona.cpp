/**
 * @brief Implementación de la clase Persona 
 * con constructor y destructor para gestionar 
 * datos personales validados.
 */
#include "Persona.h"
using namespace std;

Persona::Persona(string cedula, string primerNombre, string segundoNombre, string primerApellido, string segundoApellido,
                 string telefono, string direccion, Fecha fechaNacimiento)
    : cedula(cedula), primerNombre(primerNombre), segundoNombre(segundoNombre), primerApellido(primerApellido),
      segundoApellido(segundoApellido), telefono(telefono), direccion(direccion), fechaNacimiento(fechaNacimiento) {
    try {
        ValidacionDatos::validarCedula(cedula);
        ValidacionDatos::validarNombre(primerNombre);
        ValidacionDatos::validarNombre(segundoNombre);
        ValidacionDatos::validarNombre(primerApellido);
        ValidacionDatos::validarNombre(segundoApellido);
        ValidacionDatos::validarTelefono(telefono);
        ValidacionDatos::validarFecha(fechaNacimiento, true);
    } catch (const BancoException& e) {
        throw;
    }
}

Persona::~Persona() {}