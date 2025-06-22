#include "CodificacionCesar.h"
#include <fstream>

bool CodificacionCesar::codificarArchivo(const std::string& archivoEntrada, const std::string& archivoSalida, int clave) {
    std::ifstream entrada(archivoEntrada, std::ios::in | std::ios::binary);
    if (!entrada.is_open())
        return false;

    std::ofstream salida(archivoSalida, std::ios::out | std::ios::binary);
    if (!salida.is_open()) {
        entrada.close();
        return false;
    }

    char c;
    while (entrada.get(c)) {
        if (c >= 'A' && c <= 'Z') {
            c = ((c - 'A' + clave) % 26 + 26) % 26 + 'A'; // modulo para claves negativas
        } else if (c >= 'a' && c <= 'z') {
            c = ((c - 'a' + clave) % 26 + 26) % 26 + 'a';
        }
        salida.put(c);
    }

    entrada.close();
    salida.close();
    return true;
}

bool CodificacionCesar::decodificarArchivo(const std::string& archivoEntrada, const std::string& archivoSalida, int clave) {
    // Decodificar es codificar con clave negativa
    return codificarArchivo(archivoEntrada, archivoSalida, -clave);
}
