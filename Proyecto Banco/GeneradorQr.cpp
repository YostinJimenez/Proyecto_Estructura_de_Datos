// Incluye el archivo de encabezado que declara nuestras funciones
#include "GeneradorQr.h"

// Inclusiones de librerías estándar de C++
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>

// Inclusiones específicas de las librerías externas
#include <qrencode.h>
#include <png.h>

using namespace std;

// --- Implementación de Funciones Auxiliares ---

/**
 * @brief Guarda la matriz del código QR en un archivo PNG.
 */
bool guardarQrComoPng(const QRcode *codigoQr, const string& nombreArchivo, int escala) {
    if (!codigoQr) {
        cerr << "Error (guardarQrComoPng): No se ha generado el código QR." << endl;
        return false;
    }

    int ancho = codigoQr->width;
    int anchoEscalado = ancho * escala;
    int altoEscalado = ancho * escala;

    FILE *archivo = nullptr;
    png_structp png_ptr = nullptr;
    png_infop info_ptr = nullptr;
    png_bytep fila = nullptr;

    archivo = fopen(nombreArchivo.c_str(), "wb");
    if (!archivo) {
        cerr << "Error (guardarQrComoPng): No se pudo abrir el archivo " << nombreArchivo << " para escritura." << endl;
        return false;
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        cerr << "Error (guardarQrComoPng): png_create_write_struct falló." << endl;
        fclose(archivo);
        return false;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        cerr << "Error (guardarQrComoPng): png_create_info_struct falló." << endl;
        png_destroy_write_struct(&png_ptr, nullptr);
        fclose(archivo);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        cerr << "Error (guardarQrComoPng): Error durante la inicialización/escritura de PNG." << endl;
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(archivo);
        return false;
    }

    png_init_io(png_ptr, archivo);

    png_set_IHDR(png_ptr, info_ptr, anchoEscalado, altoEscalado,
                8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    fila = (png_bytep) malloc(3 * anchoEscalado * sizeof(png_byte));
    if (!fila) {
        cerr << "Error (guardarQrComoPng): Falló la asignación de memoria para la fila." << endl;
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(archivo);
        return false;
    }

    for (int y = 0; y < ancho; y++) {
        for (int i = 0; i < escala; ++i) {
            for (int x = 0; x < ancho; x++) {
                unsigned char moduloEsNegro = codigoQr->data[y * ancho + x] & 0x01;
                unsigned char valorColor = moduloEsNegro ? 0 : 255;
                for (int j = 0; j < escala; ++j) {
                    int indicePixel = (x * escala + j) * 3;
                    fila[indicePixel] = valorColor;
                    fila[indicePixel + 1] = valorColor;
                    fila[indicePixel + 2] = valorColor;
                }
            }
            png_write_row(png_ptr, fila);
        }
    }

    png_write_end(png_ptr, nullptr);

    if (fila) free(fila);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(archivo);

    return true;
}

/**
 * @brief Intenta convertir un archivo de imagen a PDF usando ImageMagick.
 */
bool convertirImagenAPdf(const string& nombreArchivoEntrada, const string& nombreArchivoSalida) {
    string comando = "convert \"" + nombreArchivoEntrada + "\" \"" + nombreArchivoSalida + "\"";
    cout << "Intentando generar PDF usando ImageMagick: " << comando << endl;
    
    int resultado = system(comando.c_str());
    
    if (resultado == 0) {
        cout << "PDF generado exitosamente: " << nombreArchivoSalida << endl;
        return true;
    } else {
        cerr << "Error al generar PDF con ImageMagick (código: " << resultado << ")." << endl;
        cerr << "Asegúrate de que ImageMagick esté instalado y en tu PATH." << endl;
        cerr << "Puedes instalarlo en macOS con: brew install imagemagick" << endl;
        return false;
    }
}

// --- Implementación de la Función Principal para Generar el QR ---

/**
 * @brief Genera un código QR con información personal y lo guarda como PNG y PDF.
 */
bool generarQrPersonal(
    const string& nombres,
    const string& apellidos,
    const string& cuentaBancaria,
    const string& nombreArchivoPngSalida,
    const string& nombreArchivoPdfSalida,
    int escalaPixel
) {
    // 1. Combinar la información en un solo string
    string datosCompletos = "Nombre: " + nombres + " " + apellidos + "\nCuenta: " + cuentaBancaria;
    cout << "\nContenido del QR: " << datosCompletos << endl;

    // 2. Generar el código QR usando libqrencode
    QRcode *codigoQr = QRcode_encodeString(datosCompletos.c_str(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);
    if (!codigoQr) {
        cerr << "Error: No se pudo generar el código QR con libqrencode." << endl;
        return false;
    }

    // 3. Guardar el código QR como un archivo PNG
    if (!guardarQrComoPng(codigoQr, nombreArchivoPngSalida, escalaPixel)) {
        cerr << "Error: Falló al guardar el archivo PNG." << endl;
        QRcode_free(codigoQr); // Liberar memoria antes de salir
        return false;
    }
    cout << "Archivo PNG guardado como: " << nombreArchivoPngSalida << endl;

    // 4. Convertir el archivo PNG a PDF
    if (!convertirImagenAPdf(nombreArchivoPngSalida, nombreArchivoPdfSalida)) {
        cerr << "Advertencia: No se pudo convertir el PNG a PDF. El PNG fue generado correctamente." << endl;
        // Se puede decidir si esto es un error fatal o no.
        // En este caso, no lo consideramos fatal y no retornamos 'false'.
    }

    // 5. Liberar la memoria usada por la estructura del QR
    QRcode_free(codigoQr);

    return true; // Éxito en la generación del QR y PNG
}