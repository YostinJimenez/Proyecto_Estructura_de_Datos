#include <iostream>
#include <conio.h>
#include "menuAdmin.h"
using namespace std;

void realizarBackup() {
    system("cls");
    cout << "=== REALIZAR BACKUP ===\n";
    cout << "Ejecutando backup del sistema...\n";
    // Aquí va la lógica para realizar el backup
    cout << "\nPresiona una tecla para volver...";
    getch();
}

void cifrarDatos() {
    system("cls");
    cout << "=== CIFRAR DATOS ===\n";
    cout << "Cifrando datos del sistema...\n";
    // Aquí va la lógica para cifrar datos
    cout << "\nPresiona una tecla para volver...";
    getch();
}

void mostrarMenuAdmin() {
    const char* opciones[] = {
        "Backup",
        "Cifrar",
        "Salir"
    };
    int seleccion = 0;
    int tecla;
    bool salir = false;

    while (!salir) {
        system("cls");
        cout << "=== PANEL ADMINISTRADOR ===\n\n";

        for (int i = 0; i < 3; ++i) {
            if (i == seleccion)
                cout << "==> ";
            else
                cout << "    ";
            cout << opciones[i] << endl;
        }

        tecla = getch();
        if (tecla == 224) {
            tecla = getch();
            if (tecla == 72) seleccion = (seleccion - 1 + 3) % 3; // Flecha arriba
            if (tecla == 80) seleccion = (seleccion + 1) % 3;     // Flecha abajo
        } else if (tecla == 13) { // Enter
            system("cls");
            switch (seleccion) {
                case 0: // Backup
                    realizarBackup();
                    break;
                case 1: // Cifrar
                    cifrarDatos();
                    break;
                case 2: // Salir
                    salir = true;
                    break;
            }
        }
    }
}