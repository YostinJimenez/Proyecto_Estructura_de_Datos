/**
 * @brief Gestiona y opera con fechas, permitiendo la 
 * creación y manipulación de objetos que representan 
 * días, meses y años. 
 * @param dia Día de la fecha, un valor entero entre 1 y 
 * 31 según el mes.
 * @param mes Mes de la fecha, un valor entero entre 1 y 12.
 * @param anio Año de la fecha, un valor entero mayor o 
 * igual a 1900.
 * Proporciona métodos para validar fechas, convertirlas a string, compararlas y obtener sus componentes individuales.
 */
#include "Fecha.h"
#include <ctime>
#include <sstream>

Fecha::Fecha() {
    time_t t = time(nullptr);
    tm* now = localtime(&t);
    dia = now->tm_mday;
    mes = now->tm_mon + 1;
    anio = now->tm_year + 1900;
}

Fecha::Fecha(int d, int m, int a) : dia(d), mes(m), anio(a) {}

bool Fecha::esValida() const {
    try {
        if (dia < 1 || dia > 31 || mes < 1 || mes > 12 || anio < 1900) {
            throw BancoException("Fecha invalida.");
        }
        return true;
    } catch (const BancoException& e) {
        throw;
    }
}

string Fecha::toString() const {
    ostringstream oss;
    if (dia < 10) oss << "0";
    oss << dia << "/";
    if (mes < 10) oss << "0";
    oss << mes << "/" << anio;
    return oss.str();
}

bool Fecha::esMayorQue(const Fecha& otra) const {
    if (anio != otra.anio) return anio > otra.anio;
    if (mes != otra.mes) return mes > otra.mes;
    return dia > otra.dia;
}

int Fecha::getDia() const { return dia; }
int Fecha::getMes() const { return mes; }
int Fecha::getAnio() const { return anio; }
