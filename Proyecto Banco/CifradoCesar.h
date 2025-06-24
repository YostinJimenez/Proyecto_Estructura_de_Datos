#ifndef CIFRADOCESAR_H
#define CIFRADOCESAR_H
#include "BancoException.h"
#include "ValidacionDatos.h"
#include "Hash.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>
namespace fs = std::filesystem;
using namespace std;

class CifradoCesar {
public:
    static bool codificarArchivo(const string& archivoEntrada, const string& archivoSalida, int clave);
    static bool decodificarArchivo(const string& archivoEntrada, const string& archivoSalida, int clave);
    static void listarArchivosTxt(int index);
    static void descifrar_archivos_txt();
    static void cifrar_archivos_txt();
    static bool verificarIntegridadCifrados();
    static bool verificarIntegridad(const string& archivoCifrado);
};

#endif