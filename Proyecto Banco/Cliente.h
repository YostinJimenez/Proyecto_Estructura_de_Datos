/**
 * @brief Define una clase Cliente que hereda de Persona, 
 * gestionando datos personales y bancarios del cliente.
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
 * Incluye métodos para mostrar información, obtener y modificar datos como cuenta, contraseña y serializar la información.
 */
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
    string serializar() const;
    ~Cliente();
};
#endif