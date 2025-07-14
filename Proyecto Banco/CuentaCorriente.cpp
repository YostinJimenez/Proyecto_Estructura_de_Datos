/**
 * @brief Implementa las operaciones de una cuenta corriente, incluyendo depósitos, retiros y resumen, con manejo de sobregiro.
 * @param numero Número identificador de la cuenta.
 * @param c Puntero al cliente asociado a la cuenta.
 * @param fecha Fecha de la transacción o creación.
 * @param sobregiro Límite de sobregiro permitido para la cuenta.
 * Permite depositar fondos, retirar hasta el saldo más el sobregiro, y genera un resumen con el saldo actual.
 * Incluye validación de montos y redondeo a dos decimales.
 */
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
        string tipo = esTransferencia ? "Transferencia desde " + cuentaOrigen : "Depósito";
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