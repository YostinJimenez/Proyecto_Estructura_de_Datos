/**
 * @brief Función principal del sistema bancario.
 * 
 * Muestra un menú principal con opciones para ingresar como administrador o usuario,
 * crear una nueva cuenta, recuperar credenciales o salir del sistema.
 * Controla el flujo principal del programa llamando a las funciones correspondientes
 * según la selección del usuario.
 */

#include <iostream>
#include <conio.h>
#include "menuAdmin.h"
#include "Banco.h"
#include "crearCuenta.h"
#include "menuUsuario.h"
#include "recuperarCuenta.h"
using namespace std;

int main() {
    Banco banco; // Crear instancia del banco
    const char* opciones[] = {
        "Ingresar usuario",
        "Ingresar contraseña",
        "Crear cuenta",
        "Olvide mi contraseña o usuario",
        "Salir"
    };
    int contador = 5; 
    const int numOpciones = 5;
    const char *titulo = "\n\t\t=== MENÚ PRINCIPAL ===\n\n";
    bool salir = false;
    while (!salir) {
        system("cls");
        cout << "\n\t\t  _________________________________\n";
        cout << "\t\t  ||        BANCO CONFIANZA      ||\n";
        cout << "\t\t  ||    Bienvenido al sistema    ||\n";
        cout << "\t\t  ||_____________________________||\n";

        // Usamos seleccionar_opcion para mostrar el menú y obtener la selección
        int seleccion = banco.seleccionar_opcion(titulo, opciones, numOpciones, contador);
        system("cls");
        switch (seleccion) {
            case 0: // Ingresar usuario (admin)
                mostrarMenuAdmin();
                break;
            case 1: // Ingresar contraseña (usuario)
                mostrarMenuUsuario();
                break;
            case 2: // Crear cuenta
                crearCuenta();
                break;
            case 3: // Olvide mi contraseña o usuario
                recuperarCuenta();
                break;
            case 4: // Salir
                salir = true;
                cout << "Saliendo del sistema...\n";
                break;
        }
    }
    return 0;
}
