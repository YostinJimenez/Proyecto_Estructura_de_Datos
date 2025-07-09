/**
 * @brief Proporciona funciones utilitarias para manejar la consola,
 *        como posicionar el cursor en coordenadas específicas.
 */
#pragma once
#include <windows.h>
/**
 * @brief Proporciona funciones utilitarias para manejar la consola,
 *        como posicionar el cursor en coordenadas específicas.
 */

namespace Utilidades {
    inline void gotoxy(int x, int y) {
        COORD coord;
        coord.X = x;
        coord.Y = y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }
}