/**
 * @brief Representa una transacción bancaria que puede incluir depósitos, retiros o transferencias.
 *
 * @tparam T Tipo de dato para el monto (puede ser float, double, etc.).
 * @param tipo Tipo de transacción ("Depósito", "Retiro", etc.).
 * @param monto Monto de la transacción.
 * @param fecha Fecha en la que se realiza la transacción.
 * @param numCuenta Número de cuenta asociada a la transacción.
 * @param esTransferencia Indica si la transacción es una transferencia (por defecto false).
 * @param cuentaRelacionada Cuenta de destino o fuente en caso de transferencia (opcional).
 */
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