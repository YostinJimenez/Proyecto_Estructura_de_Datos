#include <iostream> 
#include <string>   
#include "generadorQr.h"

// Usamos el espacio de nombres estándar para mayor comodidad en main.
using namespace std;

int main() {
    cout << "--- Demostracion de Generador de Codigos QR Personal ---" << endl;

    // --- Ejemplo de Uso Sencillo ---
    // Llamamos a la función principal para generar el QR con datos de ejemplo.
    // Los nombres de archivo de salida (.png y .pdf) se usarán por defecto ("qrCode.png", "qrCode.pdf").
    // La escala de píxeles también será la por defecto (5).
    cout << "\nGenerando QR para: Juan Perez, Cuenta: ES1234567890123456789012" << endl;
    bool exitoGeneracion = generarQrPersonal("Juan","Perez","ES1234567890123456789012");

    // Verificamos si la generación fue exitosa
    if (exitoGeneracion) {
        cout << "¡El codigo QR y los archivos PNG/PDF se generaron correctamente!" << endl;
        cout << "Busca 'qrCode.png' y 'qrCode.pdf' en el directorio del programa." << endl;
    } else {
        cerr << "Hubo un problema al generar el codigo QR o los archivos asociados." << endl;
    }

    cout << "\n--- Fin de la demostracion ---" << endl;
    return 0; // El programa termina con éxito
}