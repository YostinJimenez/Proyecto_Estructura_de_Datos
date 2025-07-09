/**
 * @brief Clase para crear archivos 
 * PDF con datos personales y tablas a 
 * partir de texto plano, gestionando objetos 
 * y contenido del documento.
 */
#include "Banco.h"
#include "Cliente.h"
#include "Utilidades.h"
#include <fstream>
#include <vector>
#include <string>
#include <conio.h>
#include <iostream>
#include <filesystem>
#include <iomanip>
namespace fs = std::filesystem;

class PDFCreator {
public:
    /**
     * @brief Constructor de PDFCreator que inicializa el archivo PDF y reserva los objetos necesarios.
     * @param filename Nombre del archivo PDF a crear.
     */
    PDFCreator(const std::string& filename) : file(filename, std::ios::binary), current_object_number(1) {
        offsets.push_back(0); // Objeto 0 no se usa
        write_header();
        // Reservar números de objeto
        catalog_obj = reserve_object();
        pages_obj = reserve_object();
        font_obj = reserve_object();
        // Añadir el objeto de fuente
        add_object(font_obj, "<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>");
    }

    /**
     * @brief Constructor por defecto que no hace nada.
     */
    PDFCreator() : current_object_number(1) {}

    /**
     * @brief Destructor que cierra el archivo PDF.
     */
    ~PDFCreator() {
        file.close();
    }
    /**
     * @brief Inicia una nueva página en el PDF.
     * Limpia el contenido actual de la página.
     */
    void start_page() {
        current_stream.clear();
    }

    /**
     * @brief Finaliza la página actual y guarda su contenido.
     * Añade el contenido de la página a la lista de páginas.
     */
    void end_page() {
        page_contents.push_back(current_stream);
    }

    /**
     * @brief Reserva un número de objeto para el PDF.
     * @return El número de objeto reservado.
     */
    int reserve_object() {
        int obj_num = current_object_number++;
        offsets.push_back(0);
        return obj_num;
    }

    /**
     * @brief Añade un objeto al PDF con su contenido y actualiza el offset.
     * @param obj_num Número del objeto a añadir.
     * @param content Contenido del objeto en formato PDF.
     */
    void add_object(int obj_num, const std::string& content) {
        size_t offset = file.tellp();
        file << obj_num << " 0 obj\n" << content << "\nendobj\n";
        offsets.at(obj_num) = offset;
    }

    /**
     * @brief Escribe el encabezado del PDF.
     */
    void write_header() {
        file << "%PDF-1.4\n";
    }

    /**
     * @brief Añade texto al contenido del PDF.
     * @param text Texto a añadir.
     * @param x Posición x donde se dibuja el texto.
     * @param y Posición y donde se dibuja el texto.
     * @param size Tamaño de la fuente.
     */
    void add_text(const std::string& text, float x, float y, float size) {
    current_stream += "BT /F1 " + std::to_string(size) + " Tf " +
                      std::to_string(x) + " " + std::to_string(y) +
                      " Td (" + text + ") Tj ET\n";
    }
    /**
     * @brief Añade una línea al contenido del PDF.
     * @param x1 Coordenada x inicial.
     * @param y1 Coordenada y inicial.
     * @param x2 Coordenada x final.
     * @param y2 Coordenada y final.
     */

    void add_line(float x1, float y1, float x2, float y2) {
        current_stream += std::to_string(x1) + " " + std::to_string(y1) + " m " +
                        std::to_string(x2) + " " + std::to_string(y2) + " l S\n";
    }
    
    /**
     * @brief Finaliza el PDF, escribiendo los objetos y el catálogo.
     * Este método crea los objetos de contenido, páginas y catálogo, y escribe el XREF y trailer al final del archivo PDF.
     */
    void finalize() {
    // Fuente
        font_obj = reserve_object();
        add_object(font_obj, "<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>");

        // Objetos de contenido y página
        for (const auto& content : page_contents) {
            int content_obj = reserve_object();
            int page_obj = reserve_object();
            content_objs.push_back(content_obj);
            page_objs.push_back(page_obj);

            std::string content_stream = "<< /Length " + std::to_string(content.size()) +
                                        " >>\nstream\n" + content + "\nendstream";
            add_object(content_obj, content_stream);

            std::string page_content = "<< /Type /Page /Parent " + std::to_string(pages_obj) +
                                    " 0 R /Resources << /Font << /F1 " + std::to_string(font_obj) +
                                    " 0 R >> >> /MediaBox [0 0 612 792] /Contents " +
                                    std::to_string(content_obj) + " 0 R >>";
            add_object(page_obj, page_content);
        }

        // Objeto de páginas
        std::string kids;
        for (int page_obj : page_objs)
            kids += std::to_string(page_obj) + " 0 R ";
        pages_obj = reserve_object();
        std::string pages_content = "<< /Type /Pages /Kids [" + kids + "] /Count " +
                                std::to_string(page_objs.size()) + " >>";
        add_object(pages_obj, pages_content);

        // Catálogo
        catalog_obj = reserve_object();
        std::string catalog_content = "<< /Type /Catalog /Pages " + std::to_string(pages_obj) + " 0 R >>";
        add_object(catalog_obj, catalog_content);

        // XREF y trailer
        size_t xref_offset = file.tellp();
        file << "xref\n0 " << current_object_number << "\n";
        file << "0000000000 65535 f \n";
        for (int i = 1; i < current_object_number; ++i) {
            file << std::setfill('0') << std::setw(10) << offsets[i] << " 00000 n \n";
        }
        file << "trailer\n<< /Size " << current_object_number << " /Root " << catalog_obj <<
                " 0 R >>\nstartxref\n" << xref_offset << "\n%%EOF\n";
    }

    /**
     * @brief Crea un PDF personalizado con los datos de un cliente y lo guarda en la carpeta indicada.
     * @param cliente Puntero al objeto Cliente con los datos a incluir en el PDF.
     * @param carpeta Carpeta de destino para el PDF.
     * @param rutaFotoPng Ruta de la foto en formato PNG.
     */
    void crearPDFPersonal(Cliente* cliente, std::string carpeta, const std::string& rutaFotoPng) {
    std::string nombres = cliente->getPrimerNombre() + " " + cliente->getSegundoNombre();
    std::string apellidos = cliente->getPrimerApellido() + " " + cliente->getSegundoApellido();
    std::string cuenta = cliente->getCuenta()->getNumero();
    std::string cedula = cliente->getCedula();
    std::string telefono = cliente->getTelefono();
    std::string correo = cliente->getCorreo();
    std::string fechaNac = cliente->getFechaNacimiento().toString();
    std::string archivoPdf = carpeta + "\\QRL_" + cedula + ".pdf";

    if (std::filesystem::exists(archivoPdf)) {
            Utilidades::gotoxy(10, 10);
            std::cerr << "========================================================" << std::endl;
            Utilidades::gotoxy(10, 11);
            std::cerr << "======= EL ARCHIVO " << "QRL_" + cedula + ".pdf" << " YA EXISTE ========" << std::endl;
            Utilidades::gotoxy(10, 12);
            std::cerr << "========================================================" << std::endl;
            Utilidades::gotoxy(10, 14);
            getch();
            return;
    }
    PDFCreator pdf(archivoPdf);

    float x = 50, y = 750, font_size = 12, line_height = 18;

    // Título principal
    pdf.add_text("BANCO XYZ - FORMULARIO DE DATOS PERSONALES", x, y, 16);
    y -= line_height * 2;

    // Espacio para la foto (solo recuadro y texto, no imagen real)
    float foto_x = 400, foto_y = 730, foto_w = 100, foto_h = 120;
    pdf.add_line(foto_x, foto_y, foto_x + foto_w, foto_y); // arriba
    pdf.add_line(foto_x, foto_y, foto_x, foto_y - foto_h); // izq
    pdf.add_line(foto_x + foto_w, foto_y, foto_x + foto_w, foto_y - foto_h); // der
    pdf.add_line(foto_x, foto_y - foto_h, foto_x + foto_w, foto_y - foto_h); // abajo
    pdf.add_text("FOTO", foto_x + 30, foto_y - 60, 14);
    pdf.add_text(rutaFotoPng, foto_x + 5, foto_y - foto_h - 10, 7); // ruta del PNG

    // Sección de datos personales
    y -= 10;
    pdf.add_text("DATOS PERSONALES", x, y, 13);
    y -= line_height;

    pdf.add_text("Nombres:", x, y, font_size);
    pdf.add_text(nombres, x + 120, y, font_size);
    y -= line_height;

    pdf.add_text("Apellidos:", x, y, font_size);
    pdf.add_text(apellidos, x + 120, y, font_size);
    y -= line_height;

    pdf.add_text("Teléfono:", x, y, font_size);
    pdf.add_text(telefono, x + 120, y, font_size);
    y -= line_height;

    pdf.add_text("Correo:", x, y, font_size);
    pdf.add_text(correo, x + 120, y, font_size);
    y -= line_height;

    pdf.add_text("Fecha de Nacimiento:", x, y, font_size);
    pdf.add_text(fechaNac, x + 160, y, font_size);
    y -= line_height * 2;

    // Sección de cuenta
    pdf.add_text("DATOS DE CUENTA", x, y, 13);
    y -= line_height;

    pdf.add_text("Número de Cuenta:", x, y, font_size);
    pdf.add_text(cuenta, x + 160, y, font_size);
    y -= line_height * 2;

    // Línea para firma
    pdf.add_line(x, y, x + 250, y);
    pdf.add_text("Firma del interesado", x, y - 12, 10);

    pdf.finalize();
}
    
    /**
     * @brief Crea un archivo PDF a partir de un archivo de texto plano, mostrando su contenido en formato tabular.
     * @param txtFile Nombre del archivo de texto de entrada.
     * @param pdfFile Nombre del archivo PDF de salida.
     */
    void createPDF(const std::string& txtFile, const std::string& pdfFile) {
    std::ifstream inFile(txtFile);
    if (!inFile.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << txtFile << std::endl;
        return;
    }

    std::vector<std::vector<std::string>> tablaDatos;
    std::string line;
    while (std::getline(inFile, line)) {
        std::vector<std::string> campos;
        size_t start = 0, end;
        while ((end = line.find('|', start)) != std::string::npos) {
            campos.push_back(line.substr(start, end - start));
            start = end + 1;
        }
        campos.push_back(line.substr(start));
        if (!campos.empty()) {
            tablaDatos.push_back(campos);
        }
    }
    inFile.close();

    PDFCreator pdf(pdfFile);

    float x = 50, y = 800, font_size = 11, line_height = 16;
    bool titulo_impreso = false;

    pdf.start_page(); // INICIA LA PRIMERA PÁGINA

    for (const auto& fila : tablaDatos) {
        // Salto de página si es necesario
        if (y < 120) {
            pdf.end_page();      // Termina la página actual
            pdf.start_page();    // Inicia una nueva página
            y = 800;
            titulo_impreso = false;
        }

        if (!titulo_impreso) {
            pdf.add_text("BANCO", x, y, 18);
            y -= line_height * 2;
            pdf.add_text("Datos personales de clientes", x, y, 14);
            y -= line_height * 2;
            titulo_impreso = true;
        }

        if (fila.size() < 12) continue; // Salta si faltan campos

        pdf.add_text("----------------------------------------", x, y, font_size);
        y -= line_height;

        std::string nombre = fila[1] + " " + fila[2];
        std::string apellido = fila[3] + " " + fila[4];
        pdf.add_text("Nombre: " + nombre, x, y, font_size);
        y -= line_height;
        pdf.add_text("Apellido: " + apellido, x, y, font_size);
        y -= line_height;
        pdf.add_text("Telefono: " + fila[5], x, y, font_size);
        y -= line_height;
        pdf.add_text("Correo: " + fila[6], x, y, font_size);
        y -= line_height;
        pdf.add_text("Usuario: " + fila[7], x, y, font_size);
        y -= line_height;
        pdf.add_text("Cuenta: " + fila[9], x, y, font_size);
        y -= line_height;
        pdf.add_text("Saldo: " + fila[10], x, y, font_size);
        y -= line_height;
        pdf.add_text("Fecha Nac.: " + fila[11], x, y, font_size);
        y -= line_height;
        pdf.add_text("----------------------------------------", x, y, font_size);
        y -= line_height * 2;
    }

    pdf.end_page(); // Termina la última página
    pdf.finalize();
    std::cout << "PDF creado exitosamente: " << pdfFile << std::endl;
}

private:

    std::ofstream file;
    int current_object_number;
    std::vector<size_t> offsets;
    int catalog_obj, pages_obj, font_obj;
    std::vector<std::string> page_contents; // Cada string es el contenido de una página
    std::vector<int> page_objs;             // Números de objeto de cada página
    std::vector<int> content_objs;          // Números de objeto de cada contenido
    std::string current_stream;             // Contenido de la página actual
};