#ifndef BANCO_H
#define BANCO_H
#include <string>
#include <vector>
#include "Cliente.h"
#include "ListaDobleCircular.h"
#define TECLA_ARRIBA 72
#define TECLA_ABAJO 80
#define ENTER 13
using namespace std;

class Banco {
private:
    ListaDobleCircular<Cliente> clientes;
    enum CampoBusqueda { CEDULA, PRIMER_NOMBRE, SEGUNDO_NOMBRE, PRIMER_APELLIDO, SEGUNDO_APELLIDO, SALDO };
    
    bool mostrarConfirmacion(double monto, const string& operacion);
    string seleccionarTipoCuenta();
    void mostrarMenuUsuario(Cliente* cliente);
    void buscarPorPrefijo(CampoBusqueda campo, const string& valor, vector<Cliente*>& resultados);
    void buscarPorRangoSaldo(double valor, vector<Cliente*>& resultados);
    void ordenarCuentas(const vector<Cliente*>& resultados, CampoBusqueda campo);
    void mostrar_ayuda_tecnica();
    void mostrarBaseDatos();
    public:
    void mover_cursor_opciones(int x, int y);
    int seleccionar_opcion(const char* titulo, const char* opciones[], int n, int fila_inicio);
    Banco();
    void mover_cursor(int x, int y);
    void crearCuenta();
    void mostrarMenuUsuario();
    void mostrarMenuAdmin();
    void ejecutar();
    string generarNumeroCuenta(const string& tipoCuenta);
};

#endif