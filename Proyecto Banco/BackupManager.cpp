#include "BackupManager.h"
#include "CifradoCesar.h"
#include <fstream>
#include <sstream>
#include "CuentaAhorros.h"
#include "CuentaCorriente.h"
#include "ValidacionDatos.h"
#include <filesystem>
#include <ctime>
#include <regex>
using namespace std;
/**
 * @brief Genera un nombre de archivo para el backup basado en la fecha y hora actual.
 * @return Nombre del archivo de backup en formato "backup_DDMMYYYY_HHMMSS.txt".
 */
string BackupManager::generarNombreArchivo() {
    time_t ahora = time(nullptr);
    tm* tiempo = localtime(&ahora);
    stringstream ss;
    ss << "backup_"
       << setfill('0') << setw(2) << tiempo->tm_mday
       << setfill('0') << setw(2) << (tiempo->tm_mon + 1)
       << tiempo->tm_year + 1900 << "_"
       << setfill('0') << setw(2) << tiempo->tm_hour
       << setfill('0') << setw(2) << tiempo->tm_min
       << setfill('0') << setw(2) << tiempo->tm_sec
       << ".txt";
    return ss.str();
}

/**
 * @brief Guarda un backup de la lista de clientes en un archivo.
 * @param clientes Lista de clientes a respaldar.
 * @return Nombre del archivo de backup creado.
 */
string BackupManager::guardarBackup(const ListaDobleCircular<Cliente>& clientes) {
    try {
        string nombreArchivo = generarNombreArchivo();
        ofstream archivo(nombreArchivo); // Guardar directamente con el nombre final
        if (!archivo) throw BancoException("Error al crear archivo de backup.");
        clientes.recorrer([&](Cliente* cliente) {
            stringstream fechaNac;
            fechaNac << cliente->getFechaNacimiento().getDia() << "/"
                     << cliente->getFechaNacimiento().getMes() << "/"
                     << cliente->getFechaNacimiento().getAnio();
            string datos = cliente->getCedula() + "|" +
                           cliente->getPrimerNombre() + "|" +
                           cliente->getSegundoNombre() + "|" +
                           cliente->getPrimerApellido() + "|" +
                           cliente->getSegundoApellido() + "|" +
                           cliente->getTelefono() + "|" +
                           cliente->getCorreo() + "|" +
                           cliente->getUsuario() + "|" +
                           cliente->getContrasena() + "|" +
                           cliente->getCuenta()->getNumero() + "|" +
                           to_string(cliente->getCuenta()->getSaldo()) + "|" +
                           fechaNac.str();
            archivo << datos << "\n";
        });
        archivo.close();
        return nombreArchivo;
    } catch (const BancoException& e) {
        throw;
    }
}

/**
 * @brief Restaura un backup desde un archivo y actualiza la lista de clientes.
 * @param nombreArchivo Nombre del archivo de backup a restaurar.
 * @param clientes Lista de clientes donde se restaurarán los datos.
 * @throws BancoException Si hay un error al abrir el archivo o al procesar los datos.
 */

void BackupManager::restaurarBackup(const string& nombreArchivo, ListaDobleCircular<Cliente>& clientes) {
    try {
        ifstream archivo(nombreArchivo);
        if (!archivo) throw BancoException("Error al abrir archivo de backup: " + nombreArchivo);

        // Limpiar lista de clientes existente
        clientes.recorrer([](Cliente* c) { delete c; });
        clientes = ListaDobleCircular<Cliente>();

        string linea;
        while (getline(archivo, linea)) {
            stringstream ss(linea);
            string cedula, primerNombre, segundoNombre, primerApellido, segundoApellido, telefono, correo, usuario, contrasena, numCuenta, saldoStr, fechaNacStr;
            getline(ss, cedula, '|');
            getline(ss, primerNombre, '|');
            getline(ss, segundoNombre, '|');
            getline(ss, primerApellido, '|');
            getline(ss, segundoApellido, '|');
            getline(ss, telefono, '|');
            getline(ss, correo, '|');
            getline(ss, usuario, '|');
            getline(ss, contrasena, '|');
            getline(ss, numCuenta, '|');
            getline(ss, saldoStr, '|');
            getline(ss, fechaNacStr);

            // Validar datos
            ValidacionDatos::validarNombre(primerNombre);
            ValidacionDatos::validarNombre(segundoNombre);
            ValidacionDatos::validarNombre(primerApellido);
            ValidacionDatos::validarNombre(segundoApellido);
            ValidacionDatos::validarTelefono(telefono);
            ValidacionDatos::validarCorreo(correo);

            // Parsear fecha de nacimiento
            int d, m, a;
            char sep;
            istringstream iss(fechaNacStr);
            iss >> d >> sep >> m >> sep >> a;
            Fecha fechaNac(d, m, a);
            ValidacionDatos::validarFecha(fechaNac, true);

            // Validar formato del número de cuenta
            if (numCuenta.length() != 22 || numCuenta.substr(0, 2) != "EC" || numCuenta.substr(4, 8) != "30100001") {
                throw BancoException("Formato de numero de cuenta invalido en backup: " + numCuenta);
            }

            // Crear cliente
            Cliente* cliente = new Cliente(cedula, primerNombre, segundoNombre, primerApellido, segundoApellido,
                                           telefono, "Direccion", fechaNac, "COD" + cedula, correo, usuario, contrasena);

            // Determinar tipo de cuenta según prefijo del número de cuenta
            Cuenta* cuenta;
            string prefijo = numCuenta.substr(12, 2);
            if (prefijo == "22") { // Ahorros
                cuenta = new CuentaAhorros(numCuenta, cliente, Fecha(), 0.05, 5);
            } else if (prefijo == "23") { // Corriente
                cuenta = new CuentaCorriente(numCuenta, cliente, Fecha(), 1000.0);
            } else {
                delete cliente;
                throw BancoException("Tipo de cuenta desconocido en backup: " + numCuenta);
            }

            // Restaurar saldo
            double saldo = stod(saldoStr);
            if (saldo > 0) {
                cuenta->depositar(saldo);
            }

            cliente->setCuenta(cuenta);
            clientes.insertar(cliente);
        }
        archivo.close();
    } catch (const BancoException& e) {
        throw;
    }
}

string BackupManager::normalizar(const string& s) {
    string out = std::regex_replace(s, std::regex("\r\n"), "\n");
    // Elimina espacios al final de cada línea
    out = std::regex_replace(out, std::regex("[ \t]+(\n|$)"), "\n");
    // Elimina saltos de línea al final
    while (!out.empty() && (out.back() == '\n' || out.back() == '\r')) out.pop_back();
    return out;
}


bool BackupManager::validarArchivoExistente(const ListaDobleCircular<Cliente>& clientes) {
    stringstream ssActual;
    clientes.recorrer([&](Cliente* c) {
        ssActual << c->serializar() << "\n";
    });
    string datosActuales = normalizar(ssActual.str());

    for (const auto& entry : filesystem::directory_iterator(".")) {
        string nombre = entry.path().filename().string();
        if (nombre.find("backup_") == 0 && nombre.substr(nombre.size() - 4) == ".txt") {
            ifstream file(nombre);
            stringstream ssArchivo;
            ssArchivo << file.rdbuf();
            string contenidoBackup = normalizar(ssArchivo.str());
            if (contenidoBackup == datosActuales) {
                return true; // Ya existe un backup igual
            }
        }
    }
    return false;
}