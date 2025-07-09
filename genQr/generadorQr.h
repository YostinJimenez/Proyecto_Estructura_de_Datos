/**
 * @file generador_qr.h
 * @brief Declaraciones de funciones para generar códigos QR y convertirlos a PNG y PDF.
 *
 * Este archivo de cabecera define la interfaz para la creación de códigos QR
 * a partir de datos de texto, el almacenamiento de estos códigos como imágenes PNG
 * y su posterior conversión a formato PDF. Utiliza la biblioteca libqrencode
 * para la generación del QR y herramientas externas como ImageMagick o CUPS
 * para la conversión a PDF.
 *
 * @author Ortega Enrique, GRUPO2
 * @date 8 de julio de 2024
 * @version 1.0
 */

#ifndef GENERADOR_QR_H
#define GENERADOR_QR_H

#include <string> // Para std::string
#include <qrencode.h> //

// La inclusión de <png.h> no es estrictamente necesaria en el .h
// si solo se usa en la implementación, pero qrencode.h sí para QRcode*.

/**
 * @brief Guarda una estructura de código QR (QRcode) en un archivo de imagen PNG.
 *
 * Esta función toma el objeto QRcode generado ylo renderiza
 * como una imagen en formato PNG, aplicando un factor de escala para definir
 * el tamaño final de la imagen.
 *
 * @param[in] codigoQr Puntero constante a la estructura `QRcode` que contiene los datos del código QR.
 * No debe ser nulo.
 * @param[in] nombreArchivo Cadena con la ruta y el nombre del archivo PNG donde se guardará la imagen.
 * @param[in] escala Entero que define el tamaño en píxeles de cada módulo (punto) del código QR.
 * Por ejemplo, una escala de 5 significa que cada módulo será de 5x5 píxeles.
 *
 * @return `true` si la imagen PNG se creó y guardó exitosamente.
 * @return `false` si ocurrió un error durante el proceso (ej. `codigoQr` es nulo, no se puede abrir el archivo).
 */
bool guardarQrComoPng(const QRcode *codigoQr, const std::string& nombreArchivo, int escala);

/**
 * @brief Convierte un archivo de imagen a formato PDF.
 *
 * Intenta realizar la conversión utilizando comandos del sistema. Primero prueba con `convert`
 * de ImageMagick y, si falla, intenta con `cupsfilter`. Es necesario que al menos una de
 * estas herramientas esté instalada y accesible en el PATH del sistema.
 *
 * @param[in] nombreArchivoEntrada Ruta al archivo de imagen que se desea convertir (ej. "qr.png").
 * @param[in] nombreArchivoSalida Ruta donde se guardará el archivo PDF resultante (ej. "qr.pdf").
 *
 * @return `true` si la conversión se completó con éxito.
 * @return `false` si ambas herramientas de conversión fallan o no están disponibles.
 */
bool convertirImagenAPdf(const std::string& nombreArchivoEntrada, const std::string& nombreArchivoSalida);

/**
 * @brief Genera un código QR con datos personales y lo exporta a PNG y PDF.
 *
 * Esta es una función de alto nivel que encapsula todo el proceso:
 * 1. Concatena los datos personales en una sola cadena.
 * 2. Genera un código QR a partir de esa cadena.
 * 3. Guarda el código QR como un archivo PNG.
 * 4. Convierte el archivo PNG a un archivo PDF.
 *
 * @param[in] nombres Nombres de la persona.
 * @param[in] apellidos Apellidos de la persona.
 * @param[in] cuentaBancaria Número de cuenta bancaria o cualquier otro dato relevante.
 * @param[in] nombreArchivoPngSalida (Opcional) Nombre para el archivo PNG de salida.
 * Valor por defecto: `"qrCode.png"`.
 * @param[in] nombreArchivoPdfSalida (Opcional) Nombre para el archivo PDF de salida.
 * Valor por defecto: `"qrCode.pdf"`.
 * @param[in] escalaPixel (Opcional) Factor de escalado para la imagen PNG.
 * Valor por defecto: `5`.
 *
 * @return `true` si tanto el archivo PNG como el PDF se generaron correctamente.
 * @return `false` si ocurre algún error en cualquier paso del proceso (generación de QR, guardado de PNG o conversión a PDF).
 *
 * @note La función limpiará los archivos intermedios si la conversión a PDF tiene éxito.
 */
bool generarQrPersonal(
    const std::string& nombres,
    const std::string& apellidos,
    const std::string& cuentaBancaria,
    const std::string& nombreArchivoPngSalida = "qrCode.png",
    const std::string& nombreArchivoPdfSalida = "qrCode.pdf",
    int escalaPixel = 5
);

#endif // GENERADOR_QR_H