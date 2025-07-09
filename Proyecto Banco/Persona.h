/**
 * @brief Clase base abstracta diseñada para encapsular 
 * y gestionar los datos personales de una persona, 
 * incluyendo cédula, nombres, apellidos, teléfono, 
 * dirección y fecha de nacimiento, con métodos virtuales 
 * para su manejo.
 */
#ifndef PERSONA_H
#define PERSONA_H
#include <string>
#include "Fecha.h"
#include "BancoException.h"
#include "ValidacionDatos.h"
using namespace std;

class Persona {
protected:
    string cedula;
    string primerNombre;
    string segundoNombre;
    string primerApellido;
    string segundoApellido;
    string telefono;
    string direccion;
    Fecha fechaNacimiento;
public:
    Persona(string cedula, string primerNombre, string segundoNombre, string primerApellido, string segundoApellido,
            string telefono, string direccion, Fecha fechaNacimiento);
    virtual string mostrarInformacion() = 0;
    virtual ~Persona();
    string getCedula() const { return cedula; }
    string getPrimerNombre() const { return primerNombre; }
    string getSegundoNombre() const { return segundoNombre; }
    string getPrimerApellido() const { return primerApellido; }
    string getSegundoApellido() const { return segundoApellido; }
    string getTelefono() const { return telefono; }
    string getDireccion() const { return direccion; }
    Fecha getFechaNacimiento() const { return fechaNacimiento; }
};
#endif