/**
 * @file ArbolB.h
 * @brief Declaración de la clase ArbolB para manejar un árbol binario de búsqueda de clientes.
 * @version 1.0
 * @date 2023-10-01
 * @author Tu Nombre
 */
#ifndef ARBOL_B_H
#define ARBOL_B_H

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include "Cliente.h"

class Banco;

class ArbolB {
public:
    static void dibujarArbolGrafico(Banco::NodoBST* raiz, Banco::CampoBusqueda campo, const std::string& valorBuscado, const std::string& primerCoincidencia);
    static void liberarArbol(Banco::NodoBST* raiz);
    static void insertarNodoArbol(Banco::NodoBST*& raiz, const std::string& valor, Cliente* cliente);

private:
    static void imprimirArbolB(Banco::NodoBST* nodo, int x, int y, int separacion);
};


inline void ArbolB::dibujarArbolGrafico(Banco::NodoBST* raiz, Banco::CampoBusqueda campo, const std::string& valorBuscado, const std::string& primerCoincidencia) {
    system("cls");
    
    std::string campoStr = (campo == Banco::CEDULA ? "Cedula" :
                          campo == Banco::PRIMER_NOMBRE ? "Primer Nombre" :
                          campo == Banco::SEGUNDO_NOMBRE ? "Segundo Nombre" :
                          campo == Banco::PRIMER_APELLIDO ? "Primer Apellido" :
                          campo == Banco::SEGUNDO_APELLIDO ? "Segundo Apellido" :
                          "Saldo");
    
    std::cout << "\n";
    std::cout << "  ╔═══════════════════════════════════════════════╗\n";
    std::cout << "  ║                    ARBOL B                    ║\n";
    std::cout << "  ║              Campo: " << std::left << std::setw(20) << campoStr << "  ║\n";
    std::cout << "  ╚═══════════════════════════════════════════════╝\n\n";
    
    if (!raiz) {
        std::cout << "                    [ARBOL VACIO]\n\n";
    } else {
        std::cout << "  Estructura del arbol:\n\n";
        // Posicionar la raíz en el centro de la pantalla
        imprimirArbolB(raiz, 60, 8, 20);
    }
    
    // Posicionar cursor al final para mostrar resultado de búsqueda
    Utilidades::gotoxy(0, 25);
    std::cout << "\n  ────────────────────────────────────────────────\n";
    if (primerCoincidencia.empty()) {
        std::cout << " No encontrado: " << campoStr << " (" << valorBuscado << ")\n";
    } else {
        std::cout << " Encontrado: " << campoStr << " (" << primerCoincidencia << ")\n";
    }
    std::cout << "  ────────────────────────────────────────────────\n";
    
    std::cout << "\n  Presiona cualquier tecla para continuar...";
    getch(); // Esperar a que el usuario vea el resultado
    system("cls");
}

inline void ArbolB::imprimirArbolB(Banco::NodoBST* nodo, int x, int y, int separacion) {
    if (!nodo) return;

    // Rellenar para uniformidad visual
    std::string valorFormateado = nodo->valor;
    int largoMinimo = 10;
    if (valorFormateado.size() < largoMinimo)
        valorFormateado = std::string((largoMinimo - valorFormateado.size()) / 2, ' ') + valorFormateado +
                          std::string((largoMinimo - valorFormateado.size() + 1) / 2, ' ');

    int ancho = valorFormateado.size();
    int inicioNodo = x - (ancho + 2) / 2;

    // Dibujar nodo
    Utilidades::gotoxy(inicioNodo, y);
    std::cout << "+" << std::string(ancho, '-') << "+";

    Utilidades::gotoxy(inicioNodo, y + 1);
    std::cout << "|" << valorFormateado << "|";

    Utilidades::gotoxy(inicioNodo, y + 2);
    std::cout << "+" << std::string(ancho, '-') << "+";

    // Separación adaptable
    int nuevaSeparacion = std::max(separacion / 2, ancho + 4);

    // Hijo izquierdo
    if (nodo->izquierda) {
        int hijoX = x - nuevaSeparacion;

        Utilidades::gotoxy(hijoX, y + 3);
        std::cout << "│";

        for (int i = x - 1; i > hijoX; --i) {
            Utilidades::gotoxy(i, y + 3);
            std::cout << "─";
        }

        imprimirArbolB(nodo->izquierda, hijoX, y + 4, nuevaSeparacion);
    }

    // Hijo derecho
    if (nodo->derecha) {
        int hijoX = x + nuevaSeparacion;

        Utilidades::gotoxy(hijoX, y + 3);
        std::cout << "│";

        for (int i = x + 1; i < hijoX; ++i) {
            Utilidades::gotoxy(i, y + 3);
            std::cout << "─";
        }

        imprimirArbolB(nodo->derecha, hijoX, y + 4, nuevaSeparacion);
    }
}



inline void ArbolB::liberarArbol(Banco::NodoBST* raiz) {
    if (!raiz) return;
    liberarArbol(raiz->izquierda);
    liberarArbol(raiz->derecha);
    delete raiz;
}

inline void ArbolB::insertarNodoArbol(Banco::NodoBST*& raiz, const std::string& valor, Cliente* cliente) {
    if (!raiz) {
        raiz = new Banco::NodoBST(valor, cliente);
        return;
    }
    if (valor < raiz->valor) {
        insertarNodoArbol(raiz->izquierda, valor, cliente);
    } else {
        insertarNodoArbol(raiz->derecha, valor, cliente);
    }
}

#endif 