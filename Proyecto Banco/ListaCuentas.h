// ==== ListaCuentas.h ====
#ifndef LISTACUENTAS_H
#define LISTACUENTAS_H

#include "Nodo.h"
#include <functional>
#include <iostream>

template<typename T>
class ListaCuentas {
private:
    Nodo<T>* inicio;
    int tamano;

    int contarRecursivo(Nodo<T>* nodo, Nodo<T>* origen) const {
        if (!nodo || nodo == origen)
            return 0;
        return 1 + contarRecursivo(nodo->siguiente, origen);
    }

public:
    ListaCuentas() : inicio(nullptr), tamano(0) {}

    void insertar(T* cuenta) {
        Nodo<T>* nuevo = new Nodo<T>(cuenta);
        if (!inicio) {
            inicio = nuevo;
            inicio->siguiente = inicio->anterior = inicio;
        } else {
            Nodo<T>* ultimo = inicio->anterior;
            ultimo->siguiente = nuevo;
            nuevo->anterior = ultimo;
            nuevo->siguiente = inicio;
            inicio->anterior = nuevo;
        }
        tamano++;
    }

    void recorrer(std::function<void(T*)> accion) {
        if (!inicio) return;
        Nodo<T>* actual = inicio;
        do {
            accion(actual->dato);
            actual = actual->siguiente;
        } while (actual != inicio);
    }

    T* buscar(std::function<bool(T*)> criterio) {
        if (!inicio) return nullptr;
        Nodo<T>* actual = inicio;
        do {
            if (criterio(actual->dato))
                return actual->dato;
            actual = actual->siguiente;
        } while (actual != inicio);
        return nullptr;
    }

    int contar() const {
        return contarRecursivo(inicio ? inicio->siguiente : nullptr, inicio);
    }

};

#endif