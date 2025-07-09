/**
 * @brief Implementa la clase Cliente, heredada de Persona, 
 * manejando datos personales, acceso y cuenta asociada.
 * @param cedula Identificador único del cliente.
 * @param primerNombre Primer nombre del cliente.
 * @param segundoNombre Segundo nombre del cliente (opcional).
 * @param primerApellido Primer apellido del cliente.
 * @param segundoApellido Segundo apellido del cliente (opcional).
 * @param telefono Número de teléfono del cliente.
 * @param direccion Dirección de residencia del cliente.
 * @param fechaNacimiento Fecha de nacimiento del cliente.
 * @param codigo Código único del cliente.
 * @param correo Correo electrónico del cliente.
 * @param usuario Nombre de usuario para acceso.
 * @param contrasena Contraseña para acceso.
 * Inicializa con validación de correo, usuario y contraseña, y 
 * gestiona la cuenta asociada.
 * Incluye métodos para mostrar información, obtener/modificar 
 * datos y serializar la información del cliente.
 */
#include "Cliente.h"
#include "Cuenta.h"
#include <sstream>
using namespace std;


Cliente::Cliente(string cedula, string primerNombre, string segundoNombre, string primerApellido, string segundoApellido,
                 string telefono, string direccion, Fecha fechaNacimiento, string codigo, string correo, string usuario, string contrasena)
    : Persona(cedula, primerNombre, segundoNombre, primerApellido, segundoApellido, telefono, direccion, fechaNacimiento),
      codigoCliente(codigo), correo(correo), usuario(usuario), contrasena(contrasena), cuenta(nullptr) {
    try {
        ValidacionDatos::validarCorreo(correo);
        ValidacionDatos::validarUsuario(usuario);
        ValidacionDatos::validarContrasena(contrasena);
    } catch (const BancoException& e) {
        throw;
    }
}

Cliente::~Cliente() {
    delete cuenta;
}

string Cliente::mostrarInformacion() {
    ostringstream oss;
    oss << "Cliente: " << getPrimerNombre() << " " << getSegundoNombre() << " " << getPrimerApellido() << " " << getSegundoApellido()
        << ", Cedula: " << getCedula() << ", Telefono: " << getTelefono()
        << ", Correo: " << correo << ", Usuario: " << usuario;
    return oss.str();
}

string Cliente::getCodigo() const { return codigoCliente; }
string Cliente::getCorreo() const { return correo; }
string Cliente::getUsuario() const { return usuario; }
string Cliente::getContrasena() const { return contrasena; }
void Cliente::setContrasena(const string& nuevaContrasena) { contrasena = nuevaContrasena; }
Cuenta* Cliente::getCuenta() const { return cuenta; }
void Cliente::setCuenta(Cuenta* c) { cuenta = c; }

// En Cliente.h o Cliente.cpp
string Cliente::serializar() const {
    stringstream fechaNac;
    fechaNac << getFechaNacimiento().getDia() << "/"
             << getFechaNacimiento().getMes() << "/"
             << getFechaNacimiento().getAnio();
    string datos = getCedula() + "|" +
                   getPrimerNombre() + "|" +
                   getSegundoNombre() + "|" +
                   getPrimerApellido() + "|" +
                   getSegundoApellido() + "|" +
                   getTelefono() + "|" +
                   getCorreo() + "|" +
                   getUsuario() + "|" +
                   getContrasena() + "|" +
                   getCuenta()->getNumero() + "|" +
                   to_string(getCuenta()->getSaldo()) + "|" +
                   fechaNac.str();
    return datos;
}