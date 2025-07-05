#include "Banco.h"
#include "Marquesina.h"
Marquesina marquesina(0, 0, 150, "marquesina.html", 10);
int main() {
    system("chcp 65001 > nul");
    Banco banco;
    banco.ejecutar();
    return 0;
}