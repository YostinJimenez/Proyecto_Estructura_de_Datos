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