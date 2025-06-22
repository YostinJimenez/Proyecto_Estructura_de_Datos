#ifndef NODO_H
#define NODO_H

template<typename T>
class Nodo {
public:
    T* dato;
    Nodo* siguiente;
    Nodo* anterior;
    Nodo(T* d) : dato(d), siguiente(nullptr), anterior(nullptr) {}
};

#endif