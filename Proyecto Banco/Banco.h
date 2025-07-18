/**
 * @brief Clase principal que gestiona las operaciones bancarias,
 *  permitiendo crear cuentas, realizar transacciones,
 * buscar clientes, generar reportes y administrar el sistema 
 * bancario.
 * @param monto Monto de la operación en funciones como 
 * confirmación.
 * @param operacion Nombre de la operación 
 * (ej. "TRANSFERENCIA") en confirmación.
 * @param cliente Puntero al cliente autenticado para el 
 * menú de usuario.
 * @param campo Campo de búsqueda (CEDULA, PRIMER_NOMBRE, etc.) 
 * para buscar clientes.
 * @param valor Prefijo o valor base para la búsqueda de 
 * clientes.
 * @param resultados Vector donde se almacenan los clientes 
 * encontrados.
 * @param tipoCuenta Tipo de cuenta ("Ahorros" o "Corriente") 
 * para generar número.
 * Incluye métodos para confirmaciones, selección de cuentas, 
 * búsqueda por prefijo o saldo, ordenación,
 * ayuda técnica, base de datos, cuadros de búsqueda, 
 * documentación, QR/PDF, y ejecución del menú principal.
 */
#ifndef BANCO_H
#define BANCO_H
#include "Cliente.h"
#include "ListaDobleCircular.h"
#include <string>
#include <vector>
#include <cstdlib>
#define TECLA_ARRIBA 72
#define TECLA_ABAJO 80
#define ENTER 13
using namespace std;
/**
 * @class Banco
 * @brief Clase principal que gestiona las operaciones bancarias.
 * Permite crear cuentas, realizar transacciones, buscar clientes, generar reportes y administrar el sistema.
 */
/**
 * @class Banco
 * @brief Clase principal que gestiona las operaciones bancarias.
 * Permite crear cuentas, realizar transacciones, buscar clientes, generar reportes y administrar el sistema.
 */
class Banco {
private:
    /**
     * @brief Lista de clientes del banco.
     */
    ListaDobleCircular<Cliente> clientes;

public:
    enum CampoBusqueda { CEDULA, PRIMER_NOMBRE, SEGUNDO_NOMBRE, PRIMER_APELLIDO, SEGUNDO_APELLIDO, SALDO };
    /**
     * @brief Muestra un cuadro de confirmación para operaciones monetarias.
     * @param monto Monto de la operación.
     * @param operacion Nombre de la operación (ej: "TRANSFERENCIA").
     * @return true si el usuario acepta, false si cancela.
     */
    bool mostrarConfirmacion(double monto, const string& operacion);

    /**
     * @brief Permite al usuario seleccionar el tipo de cuenta (Ahorros/Corriente).
     * @return Tipo de cuenta seleccionado o vacío si cancela.
     */
    string seleccionarTipoCuenta();
    /**
     * @brief Pausa la marquesina global.
     */
    static void pausarMarquesina();
    /**
     * @brief Reanuda la marquesina global.
     */
    static void reanudarMarquesina();
    /**
     * @brief Detiene la marquesina global.
     */
    static void detenerMarquesina();
    /**
     * @brief Muestra el menú de usuario autenticado y permite realizar operaciones.
     * @param cliente Puntero al cliente autenticado.
     */
    void mostrarMenuUsuario(Cliente* cliente);

    /**
     * @brief Busca clientes cuyo campo comienza con un prefijo dado.
     * @param campo Campo a buscar (ej: CEDULA, PRIMER_NOMBRE).
     * @param valor Prefijo a buscar.
     * @param resultados Vector donde se almacenan los clientes encontrados.
     */
    void buscarPorPrefijo(CampoBusqueda campo, const string& valor, vector<Cliente*>& resultados);

    /**
     * @brief Busca clientes cuyo saldo está dentro de un rango ±10% del valor dado.
     * @param valor Saldo base para el rango.
     * @param resultados Vector donde se almacenan los clientes encontrados.
     */
    void buscarPorRangoSaldo(double valor, vector<Cliente*>& resultados);

    /**
     * @brief Ordena y muestra una lista de clientes según el campo especificado.
     * @param resultados Vector de clientes a ordenar.
     * @param campo Campo por el que se ordena.
     */
    void ordenarCuentas(const vector<Cliente*>& resultados, CampoBusqueda campo);

    /**
     * @brief Muestra el manual de ayuda técnica del banco.
     */
    void mostrar_ayuda_tecnica();

    /**
     * @brief Muestra la base de datos de clientes y cuentas en formato tabular.
     */
    void mostrarBaseDatos();

    /**
     * @brief Imprime un cuadro de búsqueda con el texto dado y el ancho especificado.
     * @param texto Texto a mostrar.
     * @param ancho Ancho del cuadro.
     */
    void imprimirCuadroBusqueda(const std::string& texto, int ancho);

    class NodoBST {
    public:
        string valor;
        Cliente* cliente;
        NodoBST* izquierda;
        NodoBST* derecha;
        NodoBST(const string& val, Cliente* c) : valor(val), cliente(c), izquierda(nullptr), derecha(nullptr) {}
        ~NodoBST() {}
    };
    
    /**
     * @brief Libera la memoria del árbol binario de búsqueda.
     * @param raiz Puntero al nodo raíz.
     */
    void liberarArbol(NodoBST* raiz);
    /**
     * @brief Genera la documentación del proyecto usando Doxygen y la abre en el navegador.
     */
    void generarDocumentacion();

    /**
     * @brief Permite seleccionar un cliente y generar su código QR y PDF asociado.
     */
    void generarQR();

    /**
     * @brief Permite seleccionar una opción de un menú usando el teclado (flechas, enter, esc).
     * @param titulo Título del menú.
     * @param opciones Arreglo de strings con las opciones.
     * @param n Número de opciones.
     * @param fila_inicio Fila inicial para mostrar el menú.
     * @return Índice de la opción seleccionada (1-based), o 0 si se presiona ESC.
     */
    int seleccionar_opcion(const char* titulo, const char* opciones[], int n, int fila_inicio);

    /**
     * @brief Constructor de la clase Banco. Inicializa la lista de clientes y otros recursos.
     */
    Banco();

    /**
     * @brief Crea una nueva cuenta bancaria y la agrega a la lista de clientes.
     */
    void crearCuenta();

    /**
     * @brief Muestra el menú de usuario para iniciar sesión y operar sobre su cuenta.
     */
    void mostrarMenuUsuario();

    /**
     * @brief Muestra el menú de administrador y permite gestionar el sistema.
     */
    void mostrarMenuAdmin();

    /**
     * @brief Ejecuta el menú principal del sistema bancario.
     */
    void ejecutar();

    /**
     * @brief Genera un número de cuenta único y válido para un tipo de cuenta.
     * @param tipoCuenta "Ahorros" o "Corriente".
     * @return Número de cuenta generado (formato IBAN).
     */
    string generarNumeroCuenta(const string& tipoCuenta);

};

#endif