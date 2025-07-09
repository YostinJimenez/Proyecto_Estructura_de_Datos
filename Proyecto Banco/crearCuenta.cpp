/**
 * @brief Implementa la funcionalidad para crear una nueva cuenta 
 * bancaria con validación de datos.
 * Recoge nombre, apellido, cédula, usuario y contraseña, 
 * validando que la cédula tenga 10 dígitos numéricos,
 * la contraseña al menos 6 caracteres y que ambas contraseñas 
 * coincidan. Muestra mensajes de error o éxito,
 * y espera una tecla para regresar al menú principal. 
 * Incluye funciones auxiliares para validar cédula y contraseña.
 */
#include <iostream>
#include <conio.h>
#include <string>
#include "crearCuenta.h"
using namespace std;

bool validarCedula(const string& cedula) {
    // Ejemplo: Verificar que la cédula tenga solo dígitos y una longitud específica (ajusta según necesidades)
    if (cedula.length() != 10) return false;
    for (char c : cedula) {
        if (!isdigit(c)) return false;
    }
    return true;
}

bool validarContrasena(const string& password) {
    // Ejemplo: Verificar que la contraseña tenga al menos 6 caracteres
    return password.length() >= 6;
}

void crearCuenta() {
    string nombre, apellido, cedula, user, password, repetirPassword;

    system("cls");
    cout << "=== CREACION DE CUENTA ===\n\n";
    cout << "Nombre: ";
    cin >> nombre;
    cout << "Apellido: ";
    cin >> apellido;
    cout << "Cedula: ";
    cin >> cedula;
    cout << "Usuario: ";
    cin >> user;
    cout << "Clave: ";
    cin >> password;
    cout << "Repetir clave: ";
    cin >> repetirPassword;

    // Validaciones
    if (!validarCedula(cedula)) {
        cout << "\nError: La cedula debe tener 10 digitos numericos.\n";
    } else if (!validarContrasena(password)) {
        cout << "\nError: La contrasena debe tener al menos 6 caracteres.\n";
    } else if (password != repetirPassword) {
        cout << "\nError: Las contrasenas no coinciden.\n";
    } else {
        cout << "\nCuenta creada exitosamente para " << nombre << " " << apellido << ".\n";
        // Aquí puedes agregar código para guardar los datos (ej., en un archivo o base de datos)
    }
    cout << "\nPresiona una tecla para volver al menu principal...";
    getch();
}