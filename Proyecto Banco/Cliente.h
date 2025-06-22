#ifndef CLIENTE_H
#define CLIENTE_H
#include <string>
#include "Persona.h"
#include "ValidacionDatos.h"
#include "Cuenta.h"
#include "Fecha.h"
using namespace std;

class Cliente : public Persona {
private:
    string codigoCliente;
    string correo;
    string usuario;
    string contrasena;
    Cuenta* cuenta;
public:
    Cliente(string cedula, string primerNombre, string segundoNombre, string primerApellido, string segundoApellido,
            string telefono, string direccion, Fecha fechaNacimiento, string codigo, string correo, string usuario, string contrasena);
    string mostrarInformacion() override;
    string getCodigo() const;
    string getCorreo() const;
    string getUsuario() const;
    string getContrasena() const;
    void setContrasena(const string& nuevaContrasena);
    Cuenta* getCuenta() const;
    void setCuenta(Cuenta* c);
    ~Cliente();
};
#endif