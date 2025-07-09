#ifndef GENERADORQR_H
#define GENERADORQR_H

#include <string> // Para std::string
#include <qrencode.h> //

// La inclusión de <png.h> no es estrictamente necesaria en el .h
// si solo se usa en la implementación, pero qrencode.h sí para QRcode*.

/**
 * @brief Guarda la matriz del código QR en un archivo PNG.
 * @param codigoQr Puntero a la estructura QRcode generada por libqrencode.
 * @param nombreArchivo Nombre del archivo PNG de salida.
 * @param escala Factor de escalado de los módulos del QR (ej. 5 para 5x5 píxeles por módulo).
 * @return true si el PNG se guardó correctamente, false en caso contrario.
 */
bool guardarQrComoPng(const QRcode *codigoQr, const std::string& nombreArchivo, int escala);

/**
 * @brief Intenta convertir un archivo de imagen (e.g., PNG) a PDF usando ImageMagick o cupsfilter.
 * @param nombreArchivoEntrada Nombre del archivo de imagen de entrada (ej. "imagen.png").
 * @param nombreArchivoSalida Nombre del archivo PDF de salida (ej. "documento.pdf").
 * @return true si la conversión fue exitosa, false en caso contrario.
 */
bool convertirImagenAPdf(const std::string& nombreArchivoEntrada, const std::string& nombreArchivoSalida);

/**
 * @brief Genera un código QR con información personal y lo guarda como PNG y PDF.
 * @param nombres Nombres de la persona.
 * @param apellidos Apellidos de la persona.
 * @param cuentaBancaria Número de cuenta bancaria.
 * @param nombreArchivoPngSalida (Opcional) Nombre del archivo PNG de salida. Por defecto "qrCode.png".
 * @param nombreArchivoPdfSalida (Opcional) Nombre del archivo PDF de salida. Por defecto "qrCode.pdf".
 * @param escalaPixel (Opcional) Factor de escalado de los píxeles para el PNG. Por defecto 5.
 * @return true si la generación de ambos archivos fue exitosa, false en caso de error.
 */
bool generarQrPersonal(
    const std::string& nombres,
    const std::string& apellidos,
    const std::string& cuentaBancaria,
    const std::string& nombreArchivoPngSalida = "qrCode.png",
    const std::string& nombreArchivoPdfSalida = "qrCode.pdf",
    int escalaPixel = 5
);

#endif // GENERADORQR_H