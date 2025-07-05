#include "recuperarCuenta.h"
#include "Marquesina.h"
#include <iostream>
#include <conio.h>
#include "ValidacionDatos.h"
using namespace std;
extern Marquesina marquesina;
void recuperarCuenta(ListaDobleCircular<Cliente>& clientes) {
    marquesina.pausar(); // Pausar marquesina antes de pedir datos
    system("cls");
    cout << "\n=== RECUPERAR CUENTA ===\n\n";
    try {
        string cedula = ValidacionDatos::capturarEntrada(ValidacionDatos::CEDULA, "Cedula: ", 10);
        string primerNombre = ValidacionDatos::capturarEntrada(ValidacionDatos::NOMBRE, "Primer nombre: ", 50);
        string segundoNombre = ValidacionDatos::capturarEntrada(ValidacionDatos::NOMBRE, "Segundo nombre: ", 50);
        string primerApellido = ValidacionDatos::capturarEntrada(ValidacionDatos::NOMBRE, "Primer apellido: ", 50);
        string segundoApellido = ValidacionDatos::capturarEntrada(ValidacionDatos::NOMBRE, "Segundo apellido: ", 50);
        string correo = ValidacionDatos::capturarEntrada(ValidacionDatos::CORREO, "Correo: ", 254);
        Cliente* cliente = clientes.buscar([cedula, primerNombre, segundoNombre, primerApellido, segundoApellido, correo](Cliente* c) -> bool {
            return c->getCedula() == cedula &&
                   c->getPrimerNombre() == primerNombre &&
                   c->getSegundoNombre() == segundoNombre &&
                   c->getPrimerApellido() == primerApellido &&
                   c->getSegundoApellido() == segundoApellido &&
                   c->getCorreo() == correo;
        });
        if (!cliente) {
            throw BancoException("No se encontro una cuenta con los datos proporcionados.");
        }
        string nuevaContrasena = ValidacionDatos::capturarEntrada(ValidacionDatos::CONTRASENA, "Nueva contrasena: ", 20);
        string repetirContrasena = ValidacionDatos::capturarEntrada(ValidacionDatos::CONTRASENA, "Repetir contrasena: ", 20);
        if (nuevaContrasena != repetirContrasena) {
            throw BancoException("Las contrasenas no coinciden.");
        }
        cliente->setContrasena(nuevaContrasena);
        cout << "\nContrasena actualizada con exito.\n";
    } catch (const BancoException& e) {
        cout << "\nError: " << e.what() << "\n";
    }
    cout << "\nPresiona Enter para volver...";
    _getch();
    marquesina.reanudar(); // Reanudar marquesina después de recuperar cuenta
}