/**
 * @brief Define una clase para gestionar copias de seguridad 
 * (backup) de los datos de clientes del banco.
 * @param nombreArchivo Nombre del archivo de backup a generar 
 * o restaurar.
 * @param clientes Lista de clientes a respaldar o restaurar.
 * Proporciona métodos para generar nombres de archivo, guardar 
 * copias de seguridad, restaurarlas,
 * validar si un backup existe y normalizar cadenas de texto 
 * para procesamiento.
 */
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