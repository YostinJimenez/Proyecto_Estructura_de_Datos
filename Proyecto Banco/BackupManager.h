#ifndef BACKUPMANAGER_H
#define BACKUPMANAGER_H
#include <string>
#include "ListaDobleCircular.h"
#include "Cliente.h"
#include "CifradoCesar.h"
#include "BancoException.h"
using namespace std;

class BackupManager {
public:
    static string generarNombreArchivo();
    static string guardarBackup(const ListaDobleCircular<Cliente>& clientes); // Cambiado a devolver string
    static void restaurarBackup(const string& nombreArchivo, ListaDobleCircular<Cliente>& clientes);
    static bool validarArchivoExistente(const ListaDobleCircular<Cliente>& clientes);
    static string normalizar(const string& s);
};

#endif