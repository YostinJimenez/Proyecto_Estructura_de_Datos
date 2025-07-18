/**
 * @brief Define una clase base abstracta para manejar cuentas 
 * bancarias, incluyendo saldo, transacciones y datos del 
 * cliente.
 * @param numero Número identificador único de la cuenta.
 * @param c Puntero al cliente asociado a la cuenta.
 * @param fecha Fecha de apertura de la cuenta.
 * Proporciona métodos virtuales puros para depositar, retirar 
 * y mostrar resúmenes, además de consultas de transacciones 
 * por rango de fechas, monto mínimo y general.
 * Incluye manejo de excepciones y una lista doble circular 
 * para almacenar transacciones.
 */
#ifndef CUENTA_H
#define CUENTA_H
#include <string>
#include <functional>
#include "Fecha.h"
#include "Transaccion.h"
#include "ListaDobleCircular.h"
#include "BancoException.h"
#include "ValidacionDatos.h"
using namespace std;

class Cliente; // Forward declaration

class Cuenta {
protected:
    string numeroCuenta;
    double saldo;
    Cliente* cliente;
    Fecha fechaApertura;
    ListaDobleCircular<Transaccion<double>> transacciones;
public:
    Cuenta(string numero, Cliente* c, Fecha fecha);
    virtual void depositar(double monto, Fecha fecha = Fecha(), bool esTransferencia = false, string cuentaOrigen = "") = 0;
    virtual bool retirar(double monto, Fecha fecha = Fecha(), bool esTransferencia = false, string cuentaDestino = "") = 0;
    virtual string mostrarResumen() = 0;
    virtual ~Cuenta();
    string getNumero() const;
    Cliente* getCliente() const;
    double getSaldo() const;
    Fecha getFecha() const;
    void consultaRangoFechas(Fecha inicio, Fecha fin, function<void(const Transaccion<double>*)> callback) const;
    void consultaPersonalizada(double minMonto, function<void(const Transaccion<double>*)> callback) const;
    void consultaGeneral(function<void(const Transaccion<double>*)> callback) const;
};

#endif