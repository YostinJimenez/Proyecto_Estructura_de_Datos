#include <iostream>
#include <conio.h>
#include "menuAdmin.h"
#include "crearCuenta.h"
#include "menuUsuario.h"
#include "recuperarCuenta.h"
using namespace std;

void imprimirLogo() {
    cout << "  _________________________________\n";
    cout << "  ||        BANCO CONFIANZA      ||\n";
    cout << "  ||    Bienvenido al sistema    ||\n";
    cout << "  ||_____________________________||\n";
}

int main() {
    const char* opciones[] = {
        "Ingresar usuario",
        "Ingresar clave",
        "Crear cuenta",
        "Olvide mi clave o usuario",
        "Salir"
    };
    int seleccion = 0;
    int tecla;
    bool salir = false;

    while (!salir) {
        system("cls");
        imprimirLogo();
        cout << "\n=== MENU PRINCIPAL ===\n\n";

        for (int i = 0; i < 5; ++i) {
            if (i == seleccion)
                cout << "==> ";
            else
                cout << "    ";
            cout << (i + 1) << ") " << opciones[i] << endl;
        }

        tecla = getch();
        if (tecla == 224) {
            tecla = getch();
            if (tecla == 72) seleccion = (seleccion - 1 + 5) % 5; // Flecha arriba
            if (tecla == 80) seleccion = (seleccion + 1) % 5;     // Flecha abajo
        } else if (tecla == 13) { // Enter
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
    }
    return 0;
}