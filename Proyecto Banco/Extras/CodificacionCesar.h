#ifndef CODIFICACIONCESAR_H
#define CODIFICACIONCESAR_H

#include <string>

class CodificacionCesar {
public:
    // Codifica el archivo de entrada usando cifrado César y guarda en archivo binario
    static bool codificarArchivo(const std::string& archivoEntrada, const std::string& archivoSalida, int clave);

    // Decodifica el archivo binario usando cifrado César y guarda en archivo de texto
    static bool decodificarArchivo(const std::string& archivoEntrada, const std::string& archivoSalida, int clave);
};

#endif // CODIFICACIONCESAR_H
