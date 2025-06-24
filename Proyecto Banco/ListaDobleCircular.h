#ifndef LISTADOBLECIRCULAR_H
#define LISTADOBLECIRCULAR_H
#include "Nodo.h"
#include <functional>

template<typename T>
class ListaDobleCircular {
private:
    Nodo<T>* inicio;
    int tamano;

    int contarRecursivo(Nodo<T>* nodo, Nodo<T>* origen, int contador = 0) const {
        if (!nodo || nodo == origen) return contador;
        return contarRecursivo(nodo->siguiente, origen, contador + 1);
    }

public:
    ListaDobleCircular();
    ~ListaDobleCircular();
    void insertar(T* dato);
    void recorrer(std::function<void(T*)> accion) const;
    T* buscar(std::function<bool(T*)> criterio) const;
    int contar() const;
    bool estaVacia() const;
    void eliminar(T* dato);
};

template<typename T>
ListaDobleCircular<T>::ListaDobleCircular() : inicio(nullptr), tamano(0) {}

template<typename T>
ListaDobleCircular<T>::~ListaDobleCircular() {
    if (!inicio) return;
    Nodo<T>* actual = inicio;
    do {
        Nodo<T>* temp = actual;
        actual = actual->siguiente;
        delete temp->dato;
        delete temp;
    } while (actual != inicio);
}

template<typename T>
void ListaDobleCircular<T>::insertar(T* dato) {
    Nodo<T>* nuevo = new Nodo<T>(dato);
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

template<typename T>
void ListaDobleCircular<T>::recorrer(std::function<void(T*)> accion) const {
    if (!inicio) return;
    Nodo<T>* actual = inicio;
    do {
        accion(actual->dato);
        actual = actual->siguiente;
    } while (actual != inicio);
}

template<typename T>
T* ListaDobleCircular<T>::buscar(std::function<bool(T*)> criterio) const {
    if (!inicio) return nullptr;
    Nodo<T>* actual = inicio;
    do {
        if (criterio(actual->dato)) return actual->dato;
        actual = actual->siguiente;
    } while (actual != inicio);
    return nullptr;
}

template<typename T>
int ListaDobleCircular<T>::contar() const {
    return contarRecursivo(inicio ? inicio->siguiente : nullptr, inicio);
}
template <typename T>
bool ListaDobleCircular<T>::estaVacia() const {
    return inicio == nullptr || tamano == 0;
}
// ...existing code...
template <typename T>
void ListaDobleCircular<T>::eliminar(T* dato) {
    if (!inicio) return;
    Nodo<T>* actual = inicio;
    do {
        if (actual->dato == dato) {
            if (actual->siguiente == actual) { // Solo un nodo
                delete actual->dato;
                delete actual;
                inicio = nullptr;
            } else {
                actual->anterior->siguiente = actual->siguiente;
                actual->siguiente->anterior = actual->anterior;
                if (actual == inicio) inicio = actual->siguiente;
                delete actual->dato;
                delete actual;
            }
            tamano--;
            return;
        }
        actual = actual->siguiente;
    } while (actual != inicio);
}
// ...existing code...
#endif