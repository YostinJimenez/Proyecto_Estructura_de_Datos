/**
 * @brief Implementa las operaciones de una cuenta de 
 * ahorros, incluyendo depósitos, retiros con límite mensual y 
 * resumen, con aplicación de tasa de interés.
 * @param numero Número identificador de la cuenta.
 * @param c Puntero al cliente asociado a la cuenta.
 * @param fecha Fecha de la transacción o creación.
 * @param tasa Tasa de interés aplicada a la cuenta.
 * @param limite Límite máximo de retiros permitidos por mes.
 * Permite depositar fondos, retirar hasta el saldo disponible 
 * (respetando el límite de retiros), y genera un resumen 
 * con el saldo actual.
 * Incluye validación de montos y redondeo a dos decimales.
 */
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
        monto = round(monto * 100) / 100;
        saldo += monto;
        string tipo = esTransferencia ? "Transferencia desde " + cuentaOrigen : "Depósito";
        transacciones.insertar(new Transaccion<double>(tipo, monto, fecha, numeroCuenta, esTransferencia, cuentaOrigen));
    } catch (const BancoException& e) {
        throw;
    }
}

bool CuentaAhorros::retirar(double monto, Fecha fecha, bool esTransferencia, string cuentaDestino) {
    try {
        if (!esTransferencia && retirosRealizados >= limiteMensualRetiros) {
            throw BancoException("Límite de retiros mensuales alcanzado.");
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