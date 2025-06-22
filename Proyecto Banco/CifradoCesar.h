#ifndef CIFRADOCESAR_H
#define CIFRADOCESAR_H
#include <string>
#include "BancoException.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>
using namespace std;

class CifradoCesar {
public:
    static bool codificarArchivo(const string& archivoEntrada, const string& archivoSalida, int clave);
    static bool decodificarArchivo(const string& archivoEntrada, const string& archivoSalida, int clave);
    static void listarArchivosTxt(int index);
    static void descifrar_archivos_txt();
    static void cifrar_archivos_txt();
};

#endif