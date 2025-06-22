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