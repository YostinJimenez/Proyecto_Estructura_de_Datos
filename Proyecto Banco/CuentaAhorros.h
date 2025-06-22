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