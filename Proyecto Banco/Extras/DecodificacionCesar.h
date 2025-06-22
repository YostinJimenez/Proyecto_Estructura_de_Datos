#ifndef DECODIFICACIONCESAR_H
#define DECODIFICACIONCESAR_H

#include <string>

class DecodificacionCesar {
public:
    // Decodifica un archivo binario cifrado con César y guarda el resultado en un archivo de texto
    // Retorna true si la operación fue exitosa, false en caso contrario
    static bool decodificarArchivo(const std::string& archivoEntrada, const std::string& archivoSalida, int clave);
};

#endif // DECODIFICACIONCESAR_H
