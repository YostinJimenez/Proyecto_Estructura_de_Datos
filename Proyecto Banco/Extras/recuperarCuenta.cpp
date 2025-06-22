#include <iostream>
#include <conio.h>
#include <string>
#include <fstream>
#include <algorithm>
#include "recuperarCuenta.h"
using namespace std;

bool verificarCuenta(const string& nombre, const string& apellido, const string& cedula) {
    ifstream archivo("cuentas.txt");
    if (!archivo.is_open()) {
        return false; // Archivo no encontrado
    }

    string nombreArchivo, apellidoArchivo, cedulaArchivo, user, pass;
    while (getline(archivo, nombreArchivo, ',')) {
        getline(archivo, apellidoArchivo, ',');
        getline(archivo, cedulaArchivo, ',');
        getline(archivo, user, ',');
        getline(archivo, pass);
        if (nombreArchivo == nombre && apellidoArchivo == apellido && cedulaArchivo == cedula) {
            archivo.close();
            return true; // Cuenta encontrada
        }
    }
    archivo.close();
    return false; // Cuenta no encontrada
}

void ingresarDato(string& dato, const string& campo) {
    system("cls");
    cout << "=== INGRESAR " << campo << " ===\n\n";
    cout << campo << ": ";
    cin.ignore(1, '\n'); // Limpiar buffer
    getline(cin, dato); // Permitir entradas con espacios
    cout << "\nDato ingresado. Presiona una tecla para continuar...";
    getch();
}

void recuperarCuenta() {
    const char* opciones[] = {
        "Ingresar nombre",
        "Ingresar apellido",
        "Ingresar cedula",
        "Confirmar recuperacion",
        "Cancelar"
    };
    string nombre, apellido, cedula;
    int seleccion = 0;
    int tecla;
    bool salir = false;

    while (!salir) {
        system("cls");
        cout << "=== RECUPERAR CUENTA ===\n\n";
        cout << "Nombre: " << (nombre.empty() ? "[No ingresado]" : nombre) << "\n";
        cout << "Apellido: " << (apellido.empty() ? "[No ingresado]" : apellido) << "\n";
        cout << "Cedula: " << (cedula.empty() ? "[No ingresado]" : cedula) << "\n\n";

        for (int i = 0; i < 5; ++i) {
            if (i == seleccion)
                cout << "==> ";
            else
                cout << "    ";
            cout << opciones[i] << endl;
        }

        tecla = getch();
        if (tecla == 224) {
            tecla = getch();
            if (tecla == 72) seleccion = (seleccion - 1 + 5) % 5; // Flecha arriba
            if (tecla == 80) seleccion = (seleccion + 1) % 5;     // Flecha abajo
        } else if (tecla == 13) { // Enter
            system("cls");
            switch (seleccion) {
                case 0: // Ingresar nombre
                    ingresarDato(nombre, "Nombre");
                    break;
                case 1: // Ingresar apellido
                    ingresarDato(apellido, "Apellido");
                    break;
                case 2: // Ingresar cédula
                    ingresarDato(cedula, "Cedula");
                    break;
                case 3: // Confirmar recuperación
                    if (nombre.empty() || apellido.empty() || cedula.empty()) {
                        cout << "\nError: Debes ingresar todos los datos.\n";
                        cout << "Presiona una tecla para continuar...";
                        getch();
                    } else {
                        bool cedulaValida = cedula.length() == 10 && all_of(cedula.begin(), cedula.end(), ::isdigit);
                        if (!cedulaValida) {
                            cout << "\nError: La cedula debe tener 10 digitos numericos.\n";
                        } else if (verificarCuenta(nombre, apellido, cedula)) {
                            cout << "\nSolicitud de recuperacion procesada.\n";
                            cout << "Si los datos coinciden, recibiras instrucciones por correo.\n";
                            salir = true; // Salir después de procesar
                        } else {
                            cout << "\nError: No se encontro una cuenta con los datos proporcionados.\n";
                        }
                        cout << "\nPresiona una tecla para continuar...";
                        getch();
                    }
                    break;
                case 4: // Cancelar
                    salir = true;
                    break;
            }
        }
    }
}