/**
 * @brief Define una clase para cuentas de ahorros, 
 * heredada de Cuenta, con tasa de interés y 
 * límite de retiros mensuales.
 * @param numero Número identificador de la cuenta.
 * @param c Puntero al cliente asociado a la cuenta.
 * @param fecha Fecha de creación o modificación de la cuenta.
 * @param tasa Tasa de interés aplicada a la cuenta.
 * @param limite Límite máximo de retiros permitidos por mes.
 * Incluye métodos para depositar, retirar fondos 
 * (respetando el límite) y mostrar un resumen del estado de 
 * la cuenta.
 */
#ifndef CUENTAAHORROS_H
#define CUENTAAHORROS_H
#include "Cuenta.h"

class CuentaAhorros : public Cuenta {
private:
    double tasaInteres;
    int limiteMensualRetiros;
    int retirosRealizados;
public:
    CuentaAhorros(string numero, Cliente* c, Fecha fecha, double tasa, int limite);
    void depositar(double monto, Fecha fecha = Fecha(), bool esTransferencia = false, string cuentaOrigen = "") override;
    bool retirar(double monto, Fecha fecha = Fecha(), bool esTransferencia = false, string cuentaDestino = "") override;
    string mostrarResumen() override;
};

#endif