#ifndef VALIDACION_DATOS_H
#define VALIDACION_DATOS_H
#include <string>
#include <set>
#include "Fecha.h"
#include "BancoException.h"

/**
 * @brief Clase que proporciona métodos para validar diferentes tipos de datos de entrada.
 */

class ValidacionDatos {
public:
    enum TipoDato {CEDULA, NOMBRE, CORREO, TELEFONO, USUARIO, CONTRASENA, FECHA, MONTO, NOMBRE_ARCHIVO, CEDULA_PREFIJO};
    /**
     * @brief Captura e interpreta una entrada según el tipo de dato.
     * @param tipo Tipo de dato a capturar.
     * @param mensaje Mensaje a mostrar al usuario.
     * @param maxLongitud Longitud máxima permitida.
     * @return Cadena de texto capturada y validada.
     */
    static std::string capturarEntrada(TipoDato tipo, const std::string& mensaje, size_t maxLongitud);
    /**
     * @brief Captura una fecha de forma interactiva.
     * @param mensaje Mensaje a mostrar.
     * @param esFechaNacimiento Indica si es una fecha de nacimiento (por defecto false).
     * @return Objeto Fecha ingresado.
     */
    static Fecha capturarFechaInteractiva(const std::string& mensaje, bool esFechaNacimiento = false);
    /**
     * @brief Captura una fecha interactiva con un límite mínimo.
     * @param mensaje Mensaje a mostrar.
     * @param esFechaNacimiento Indica si es una fecha de nacimiento.
     * @param minFecha Puntero a la fecha mínima permitida.
     * @return Objeto Fecha ingresado.
     */
    static Fecha capturarFechaInteractiva(const std::string& mensaje, bool esFechaNacimiento, const Fecha* minFecha);
    /**
     * @brief Valida una fecha dada.
     * @param fecha Fecha a validar.
     * @param esFechaNacimiento Indica si se trata de una fecha de nacimiento.
     */
    static void validarFecha(const Fecha& fecha, bool esFechaNacimiento = false);
    /**
     * @brief Valida una cédula de identidad.
     * @param cedula Cédula a validar.
     */
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
    static std::string validar_Fecha(const std::string& fecha);
    static std::string validarHora(const std::string& hora);
    static bool validar_hora_minuto_segundo(int hora, int minuto, int segundo);
    static bool es_fecha_valida(int dia, int mes, int anio);
    static std::string validarCaracter(std::string& opcion);
};

#endif