/**
 * @brief Clase plantilla Nodo 
 * para representar un nodo en una estructura de 
 * datos, con un dato, puntero al siguiente y anterior 
 * nodo, usada en listas doblemente enlazadas.
 */
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