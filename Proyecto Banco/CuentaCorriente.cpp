#include "CuentaCorriente.h"
#include "ValidacionDatos.h"
#include <cmath>
#include <sstream>
#include <ios>
#include <iomanip>

CuentaCorriente::CuentaCorriente(string numero, Cliente* c, Fecha fecha, double sobregiro)
    : Cuenta(numero, c, fecha), sobregiroPermitido(sobregiro) {}

void CuentaCorriente::depositar(double monto, Fecha fecha, bool esTransferencia, string cuentaOrigen) {
    try {
        ValidacionDatos::validarMonto(monto, false);
        monto = round(monto * 100) / 100;
        saldo += monto;
        string tipo = esTransferencia ? "Deposito" : "Deposito";
        transacciones.insertar(new Transaccion<double>(tipo, monto, fecha, numeroCuenta, esTransferencia, cuentaOrigen));
    } catch (const BancoException& e) {
        throw;
    }
}

bool CuentaCorriente::retirar(double monto, Fecha fecha, bool esTransferencia, string cuentaDestino) {
    try {
        ValidacionDatos::validarMonto(monto, false);
        if (monto > saldo + sobregiroPermitido) {
            throw BancoException("Saldo y sobregiro insuficientes.");
        }
        monto = round(monto * 100) / 100;
        saldo -= monto;
        transacciones.insertar(new Transaccion<double>("Retiro", monto, fecha, numeroCuenta, esTransferencia, cuentaDestino));
        return true;
    } catch (const BancoException& e) {
        throw;
    }
}

string CuentaCorriente::mostrarResumen() {
    ostringstream oss;
    oss << fixed << setprecision(2);
    oss << "Cuenta Corriente N°" << numeroCuenta << ", Saldo: $" << saldo;
    return oss.str();
}