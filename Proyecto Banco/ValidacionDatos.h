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
    /**
     * @brief Enumera los tipos de datos válidos para la entrada del usuario.
     * @details
     * - CEDULA: Validación de cédula de identidad.
     * - NOMBRE: Validación de nombres.
     * - CORREO: Validación de correos electrónicos.
     * - TELEFONO: Validación de números telefónicos.   
     * - USUARIO: Validación de nombres de usuario.
     * - CONTRASENA: Validación de contraseñas.
     * - FECHA: Validación de fechas.
     * - MONTO: Validación de montos monetarios.
     * - CEDULAPREFIJO: Validación de prefijos de cédula.
     * - NUMERODECUENTA: Validación de números de cuenta bancaria.
     */
    enum TipoDato {CEDULA, NOMBRE, APELLIDO, CORREO, TELEFONO, USUARIO, CONTRASENA, FECHA, MONTO, CEDULA_PREFIJO, NUM_CUENTA};
    
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
    /**
     * @brief Valida un prefijo de cédula.
     * @param cedula Prefijo de cédula a validar.
     */
    static void validarCedulaPrefijo(const std::string& cedula);
    /**
     * @brief Valida un número de cuenta de 10 dígitos (comenzando con 22 o 23).
     * @param numCuenta Número de cuenta a validar.
     */
    static void validarNumCuenta(const std::string& numCuenta);
    /**
     * @brief Limpia la línea actual en la consola.
     * @param mensaje Mensaje opcional a mostrar después de limpiar la línea.
     */
    static void limpiar_linea(const std::string& mensaje);
    /** @brief Valida un nombre. 
     * @param nombre Nombre a validar.
     * @throws BancoException Si el nombre es inválido.
    */
    static void validarNombre(const std::string& nombre);
    /**
     * @brief Valida un correo electrónico.
     * @param correo Correo a validar.
     * @throws BancoException Si el correo es inválido.
     */
    static void validarCorreo(const std::string& correo);
    /**
     * @brief Valida un número de teléfono.
     * @param telefono Teléfono a validar.
     * @throws BancoException Si el teléfono es inválido.
     */
    static void validarTelefono(const std::string& telefono);
    /**
     * @brief Valida un nombre de usuario.
     * @param usuario Usuario a validar.
     * @throws BancoException Si el usuario es inválido.
     */
    static void validarUsuario(const std::string& usuario);
    /**
     * @brief Valida una contraseña.
     * @param contrasena Contraseña a validar.
     * @throws BancoException Si la contraseña es inválida.
     */
    static void validarContrasena(const std::string& contrasena);
    /**
     * @brief Valida un monto de dinero.
     * @param montoStr Monto a validar como cadena.
     * @param permitirCero Indica si se permite un monto de cero.
     * @throws BancoException Si el monto es inválido.
     */
    static void validarMonto(const std::string& montoStr, bool permitirCero);
    /**
     * @brief Valida un monto de dinero.
     * @param monto Monto a validar.
     * @param permitirCero Indica si se permite un monto de cero.
     * @throws BancoException Si el monto es inválido.
     */
    static void validarMonto(double monto, bool permitirCero);
    /**
     * @brief Valida un apellido.
     * @param entrada Apellido a validar.
     * @throws BancoException Si el apellido es inválido.
     */
    static void validarApellido(const std::string& entrada);
    /**
     * @brief Valida un DNI (Documento Nacional de Identidad).
     * @param mensaje Mensaje a mostrar al usuario.
     * @return Cadena de texto con el DNI validado.
     * @throws BancoException Si el DNI es inválido.
     */
    static std::string ingresar_dni(const std::string& mensaje);
    /**
     * @brief Valida una fecha.
     * @param fecha Fecha a validar.
     * @return Cadena de texto con la fecha validada.
     * @throws BancoException Si la fecha es inválida.
     */
    static std::string validar_Fecha(const std::string& fecha);
    /**
     * @brief Valida una hora.
     * @param hora Hora a validar.
     * @return Cadena de texto con la hora validada.
     * @throws BancoException Si la hora es inválida.
     */
    static std::string validarHora(const std::string& hora);
    /**
     * @brief Valida una combinación de hora, minuto y segundo.
     * @param hora Hora a validar.
     * @param minuto Minuto a validar.
     * @param segundo Segundo a validar.
     * @return true si la combinación es válida, false en caso contrario.
     */
    static bool validar_hora_minuto_segundo(int hora, int minuto, int segundo);
    /**
     * @brief Verifica si una fecha es válida.
     * @param dia Día a validar.
     * @param mes Mes a validar.
     * @param anio Año a validar.
     * @return true si la fecha es válida, false en caso contrario.
     */
    static bool es_fecha_valida(int dia, int mes, int anio);
    /**
     * @brief Valida un carácter.
     * @param opcion Carácter a validar.
     * @return Cadena de texto con el carácter validado.
     * @throws BancoException Si el carácter es inválido.
     */
    static std::string validarCaracter(std::string& opcion);
};

#endif