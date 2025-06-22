#include "CuentaAhorros.h"
#include "ValidacionDatos.h"
#include <cmath>
#include <sstream>
#include <ios>
#include <iomanip>

CuentaAhorros::CuentaAhorros(string numero, Cliente* c, Fecha fecha, double tasa, int limite)
    : Cuenta(numero, c, fecha), tasaInteres(tasa), limiteMensualRetiros(limite), retirosRealizados(0) {}

void CuentaAhorros::depositar(double monto, Fecha fecha, bool esTransferencia, string cuentaOrigen) {
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

bool CuentaAhorros::retirar(double monto, Fecha fecha, bool esTransferencia, string cuentaDestino) {
    try {
        ValidacionDatos::validarMonto(monto, false);
        if (!esTransferencia && retirosRealizados >= limiteMensualRetiros) {
            throw BancoException("Limite de retiros mensuales alcanzado.");
        }
        if (monto > saldo) {
            throw BancoException("Saldo insuficiente.");
        }
        monto = round(monto * 100) / 100;
        saldo -= monto;
        if (!esTransferencia) retirosRealizados++;
        transacciones.insertar(new Transaccion<double>("Retiro", monto, fecha, numeroCuenta, esTransferencia, cuentaDestino));
        return true;
    } catch (const BancoException& e) {
        throw;
    }
}

string CuentaAhorros::mostrarResumen() {
    ostringstream oss;
    oss << fixed << setprecision(2);
    oss << "Cuenta Ahorros N°" << numeroCuenta << ", Saldo: $" << saldo;
    return oss.str();
}