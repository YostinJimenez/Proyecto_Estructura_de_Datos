#ifndef VALIDACION_DATOS_H
#define VALIDACION_DATOS_H
#include <string>
#include <set>
#include "Fecha.h"
#include "BancoException.h"

class ValidacionDatos {
public:
    enum TipoDato {CEDULA, NOMBRE, CORREO, TELEFONO, USUARIO, CONTRASENA, FECHA, MONTO, NOMBRE_ARCHIVO, CEDULA_PREFIJO};
    static std::string capturarEntrada(TipoDato tipo, const std::string& mensaje, size_t maxLongitud);
    static Fecha capturarFechaInteractiva(const std::string& mensaje, bool esFechaNacimiento = false);
    static Fecha capturarFechaInteractiva(const std::string& mensaje, bool esFechaNacimiento, const Fecha* minFecha);
    static void validarFecha(const Fecha& fecha, bool esFechaNacimiento = false);
    static void validarCedula(const std::string& cedula);
    static void validarCedulaPrefijo(const std::string& cedula);
    static void limpiar_linea(const std::string& mensaje);
    static void validarNombre(const std::string& nombre);
    static void validarCorreo(const std::string& correo);
    static void validarTelefono(const std::string& telefono);
    static void validarUsuario(const std::string& usuario);
    static void validarContrasena(const std::string& contrasena);
    static void validarMonto(double monto, bool permitirCero);
    static void validarNombreArchivo(const std::string& entrada);
    static std::string ingresar_dni(const std::string& mensaje);
    static std::string validarFecha(const std::string& fecha);
    static std::string validarHora(const std::string& hora);
    static bool validar_hora_minuto_segundo(int hora, int minuto, int segundo);
    static bool es_fecha_valida(int dia, int mes, int anio);
    static std::string validarCaracter(std::string& opcion);
};

#endif