#include <iostream>
#include <conio.h>
#include <string>
#include "menuUsuario.h"
using namespace std;

// Estructura para almacenar datos del usuario (temporal, hasta implementar persistencia)
struct Cuenta {
    string usuario = "usuario1";
    string numeroCuenta = "0012345";
    float saldo = 1000.50;
};

void realizarTransferencia(Cuenta& cuenta) {
    system("cls");
    cout << "=== REALIZAR TRANSFERENCIA ===\n\n";
    cout << "Cuenta actual: " << cuenta.numeroCuenta << "\nSaldo: $" << cuenta.saldo << "\n\n";
    string cuentaDestino;
    float monto;
    cout << "Ingrese numero de cuenta destino: ";
    cin >> cuentaDestino;
    cout << "Ingrese monto a transferir: $";
    cin >> monto;

    if (monto > 0 && monto <= cuenta.saldo) {
        cuenta.saldo -= monto;
        cout << "\nTransferencia de $" << monto << " a la cuenta " << cuentaDestino << " realizada con exito.\n";
        cout << "Nuevo saldo: $" << cuenta.saldo << "\n";
    } else {
        cout << "\nError: Monto invalido o saldo insuficiente.\n";
    }
    cout << "\nPresiona una tecla para volver...";
    getch();
}

void realizarDeposito(Cuenta& cuenta) {
    system("cls");
    cout << "=== REALIZAR DEPOSITO ===\n\n";
    cout << "Cuenta actual: " << cuenta.numeroCuenta << "\nSaldo: $" << cuenta.saldo << "\n\n";
    float monto;
    cout << "Ingrese monto a depositar: $";
    cin >> monto;

    if (monto > 0) {
        cuenta.saldo += monto;
        cout << "\nDeposito de $" << monto << " realizado con exito.\n";
        cout << "Nuevo saldo: $" << cuenta.saldo << "\n";
    } else {
        cout << "\nError: El monto debe ser mayor a 0.\n";
    }
    cout << "\nPresiona una tecla para volver...";
    getch();
}

void mostrarMenuUsuario() {
    const char* opciones[] = {
        "Transferir",
        "Depositar",
        "Salir"
    };
    Cuenta cuenta; // Temporal, deberías pasar los datos reales del usuario autenticado
    int seleccion = 0;
    int tecla;
    bool salir = false;

    while (!salir) {
        system("cls");
        cout << "=== MENU USUARIO ===\n\n";
        cout << "Cuenta: " << cuenta.numeroCuenta << "\nUsuario: " << cuenta.usuario << "\nSaldo: $" << cuenta.saldo << "\n\n";

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
                case 0: // Transferir
                    realizarTransferencia(cuenta);
                    break;
                case 1: // Depositar
                    realizarDeposito(cuenta);
                    break;
                case 2: // Salir
                    salir = true;
                    break;
            }
        }
    }
}