/**
 * @brief Define una clase para cuentas corrientes, heredada de la clase Cuenta, que incluye un límite de sobregiro permitido.
 * @param numero Número identificador de la cuenta.
 * @param c Puntero al cliente asociado a la cuenta.
 * @param fecha Fecha de creación o modificación de la cuenta.
 * @param sobregiro Límite de sobregiro permitido para la cuenta.
 * Proporciona métodos para depositar, retirar fondos y mostrar un resumen, con soporte para transferencias y fechas personalizadas.
 */
#ifndef CUENTACORRIENTE_H
#define CUENTACORRIENTE_H
#include "Cuenta.h"

class CuentaCorriente : public Cuenta {
private:
    double sobregiroPermitido;
public:
    CuentaCorriente(string numero, Cliente* c, Fecha fecha, double sobregiro);
    void depositar(double monto, Fecha fecha = Fecha(), bool esTransferencia = false, string cuentaOrigen = "") override;
    bool retirar(double monto, Fecha fecha = Fecha(), bool esTransferencia = false, string cuentaDestino = "") override;
    string mostrarResumen() override;
};

#endif