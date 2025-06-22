#ifndef TRANSACCION_H
#define TRANSACCION_H
#include <string>
#include <sstream>
#include <iomanip>
#include "Fecha.h"
#include "BancoException.h"
#include "ValidacionDatos.h"
using namespace std;

template<typename T>
class Transaccion {
private:
    string tipo;
    T monto;
    Fecha fecha;
    string numeroCuenta;
    bool esTransferencia;
    string cuentaRelacionada;
public:
    Transaccion(string tipo, T monto, Fecha fecha, string numCuenta, bool esTransferencia = false, string cuentaRelacionada = "");
    string resumenTransaccion() const;
    Fecha getFecha() const;
    string getTipo() const;
    T getMonto() const;
    string getNumeroCuenta() const;
};

template<typename T>
Transaccion<T>::Transaccion(string tipo, T monto, Fecha fecha, string numCuenta, bool esTransferencia, string cuentaRelacionada)
    : tipo(tipo), monto(monto), fecha(fecha), numeroCuenta(numCuenta), esTransferencia(esTransferencia), cuentaRelacionada(cuentaRelacionada) {
    try {
        ValidacionDatos::validarMonto(monto, false);
        ValidacionDatos::validarFecha(fecha);
    } catch (const BancoException& e) {
        throw;
    }
}

template<typename T>
string Transaccion<T>::resumenTransaccion() const {
    ostringstream oss;
    oss << fixed << setprecision(2);
    if (esTransferencia) {
        oss << (tipo == "Retiro" ? "Transferencia Enviada" : "Transferencia Recibida") 
            << ", Monto: $" << monto << ", Fecha: " << fecha.toString() 
            << ", Cuenta: " << numeroCuenta;
        oss << (tipo == "Retiro" ? " a " : " desde ") << cuentaRelacionada;
    } else {
        oss << "Transaccion: " << tipo << ", Monto: $" << monto 
            << ", Fecha: " << fecha.toString() << ", Cuenta: " << numeroCuenta;
    }
    return oss.str();
}

template<typename T>
Fecha Transaccion<T>::getFecha() const { return fecha; }

template<typename T>
string Transaccion<T>::getTipo() const { return tipo; }

template<typename T>
T Transaccion<T>::getMonto() const { return monto; }

template<typename T>
string Transaccion<T>::getNumeroCuenta() const { return numeroCuenta; }

#endif