#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define byte win_byte
#include <windows.h>
#undef byte
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
extern std::mutex consola_mutex;

struct ElementoMarquesina {
    std::string texto;
    int colorTexto;   // Código de color del texto
    int colorFondo;   // Código de color del fondo
    bool negrita;     // Texto en negrita
};

class Marquesina {
private:
    std::vector<ElementoMarquesina> elementos;
    std::thread hiloMarquesina;
    std::atomic<bool> ejecutando;
    std::atomic<bool> pausado;
    int posX, posY;
    int ancho;
    int velocidad;
    std::string archivoHTML;
    std::mutex mtx;
    
    // Método para leer y parsear HTML simple
    void cargarDesdeHTML();
    
    // Método que ejecuta el hilo de la marquesina
    void ejecutarMarquesina();
    // Método para analizar etiquetas simples de HTML
    std::vector<ElementoMarquesina> parsearHTML(const std::string& contenido);
    // Verificar tiempo modificación archivo (usando API Windows en vez de std::filesystem)
    FILETIME ultimaModificacion;
    bool archivoModificado();
public:
    int get_cursor_row(); // Función para obtener la fila actual del cursor
    Marquesina(int x, int y, int ancho, const std::string& archivoHTML, int velocidad = 100);
    ~Marquesina();
    //void limpiarMenu(int desde, int hasta);
    void iniciar();
    void detener();
    void pausar();
    void reanudar();
    void actualizarTexto(const std::string& nuevoTexto);
    void mostrarRutaArchivo();
};