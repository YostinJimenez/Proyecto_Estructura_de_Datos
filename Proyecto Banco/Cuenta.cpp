#include "Cuenta.h"
#include <iostream>
#include <iomanip>
using namespace std;

Cuenta::Cuenta(string numero, Cliente* c, Fecha fecha)
    : numeroCuenta(numero), saldo(0), cliente(c), fechaApertura(fecha) {
    try {
        ValidacionDatos::validarFecha(fecha);
    } catch (const BancoException& e) {
        throw;
    }
}

Cuenta::~Cuenta() {}

string Cuenta::getNumero() const { return numeroCuenta; }
Cliente* Cuenta::getCliente() const { return cliente; }
double Cuenta::getSaldo() const { return saldo; }
Fecha Cuenta::getFecha() const { return fechaApertura; }

void Cuenta::consultaRangoFechas(Fecha inicio, Fecha fin, function<void(const Transaccion<double>*)> callback) const {
    int contador = 0;
    transacciones.recorrer([&](Transaccion<double>* t) {
        if (!t->getFecha().esMayorQue(fin) && (t->getFecha().esMayorQue(inicio) || t->getFecha().toString() == inicio.toString())) {
            callback(t);
            contador++;
        }
    });
    if (contador == 0) {
        cout << "\nNo se encontraron transacciones en el rango de fechas especificado.\n";
    }
}

void Cuenta::consultaPersonalizada(double minMonto, function<void(const Transaccion<double>*)> callback) const {
    int contador = 0;
    transacciones.recorrer([&](Transaccion<double>* t) {
        if (t->getMonto() >= minMonto) {
            callback(t);
            contador++;
        }
    });
    if (contador == 0) {
        cout << "\nNo se encontraron transacciones con un monto mayor o igual a $" << fixed << setprecision(2) << minMonto << ".\n";
    }
}

void Cuenta::consultaGeneral(function<void(const Transaccion<double>*)> callback) const {
    int contador = 0;
    transacciones.recorrer([&](Transaccion<double>* t) {
        callback(t);
        contador++;
    });
    if (contador == 0) {
        cout << "\nNo se encontraron transacciones en la cuenta.\n";
    }
}