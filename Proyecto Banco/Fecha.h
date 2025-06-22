#ifndef FECHA_H
#define FECHA_H
#include <string>
#include "BancoException.h"
using namespace std;

class Fecha {
private:
    int dia, mes, anio;
public:
    Fecha();
    Fecha(int d, int m, int a);
    bool esValida() const;
    string toString() const;
    bool esMayorQue(const Fecha& otra) const;
    int getDia() const;
    int getMes() const;
    int getAnio() const;
};

#endif