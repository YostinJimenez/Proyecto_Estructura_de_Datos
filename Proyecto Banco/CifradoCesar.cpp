#include "CifradoCesar.h"

bool CifradoCesar::codificarArchivo(const string& archivoEntrada, const string& archivoSalida, int clave) {
    try {
        ifstream entrada(archivoEntrada, ios::binary);
        if (!entrada.is_open()) throw BancoException("Error al abrir archivo de entrada.");
        ofstream salida(archivoSalida, ios::binary);
        if (!salida.is_open()) {
            entrada.close();
            throw BancoException("Error al abrir archivo de salida.");
        }
        char c;
        while (entrada.get(c)) {
            if (c >= 'A' && c <= 'Z') {
                c = ((c - 'A' + clave) % 26 + 26) % 26 + 'A';
            } else if (c >= 'a' && c <= 'z') {
                c = ((c - 'a' + clave) % 26 + 26) % 26 + 'a';
            }
            salida.put(c);
        }
        entrada.close();
        salida.close();
        return true;
    } catch (const BancoException& e) {
        throw;
    }
}

bool CifradoCesar::decodificarArchivo(const string& archivoEntrada, const string& archivoSalida, int clave) {
    return codificarArchivo(archivoEntrada, archivoSalida, -clave);
}

void CifradoCesar::listarArchivosTxt(int index) {
    vector<pair<string, time_t>> backupsTxt;
    int contador = 1;
    //Decifrar
    if(index == 1 ){
        for (const auto& entry : fs::directory_iterator(".")) {
            string nombre = entry.path().filename().string();
            if (nombre.find("cifrado_") == 0 && nombre.substr(nombre.size() - 4) == ".txt") {
                // Formato: cifrado_DDMMYYYY_HHMMSS.txt (sin ceros a la izquierda)
                size_t pos1 = nombre.find('_') + 1;
                size_t pos2 = nombre.find('_', pos1);
                if (pos2 == string::npos) continue;
                string fecha = nombre.substr(pos1, pos2 - pos1); // Ej: 22062025
                string hora = nombre.substr(pos2 + 1, nombre.size() - pos2 - 5); // Ej: 11033

                // Parsear fecha y hora
                int d, m, a, h, min, s;
                if (fecha.length() == 7) { // DMMYYYY
                    d = stoi(fecha.substr(0, 1));
                    m = stoi(fecha.substr(1, 1));
                    a = stoi(fecha.substr(2, 4));
                } else if (fecha.length() == 8) { // DDMMYYYY
                    d = stoi(fecha.substr(0, 2));
                    m = stoi(fecha.substr(2, 2));
                    a = stoi(fecha.substr(4, 4));
                } else {
                    continue;
                }
                if (hora.length() == 5) { // HMMSS
                    h = stoi(hora.substr(0, 1));
                    min = stoi(hora.substr(1, 2));
                    s = stoi(hora.substr(3, 2));
                } else if (hora.length() == 6) { // HHMMSS
                    h = stoi(hora.substr(0, 2));
                    min = stoi(hora.substr(2, 2));
                    s = stoi(hora.substr(4, 2));
                } else {
                    continue;
                }
                tm tm = {};
                tm.tm_mday = d;
                tm.tm_mon = m - 1;
                tm.tm_year = a - 1900;
                tm.tm_hour = h;
                tm.tm_min = min;
                tm.tm_sec = s;
                time_t tiempo = mktime(&tm);
                backupsTxt.emplace_back(nombre, tiempo);
            }
        }
    }else if (index==2){
        for (const auto& entry : filesystem::directory_iterator(".")) {
            string nombre = entry.path().filename().string();
            if (nombre.find("backup_") == 0 && nombre.substr(nombre.size() - 4) == ".txt") {
                // Formato: backup_DDMMYYYY_HHMMSS.txt (sin ceros a la izquierda)
                size_t pos1 = nombre.find('_') + 1;
                size_t pos2 = nombre.find('_', pos1);
                if (pos2 == string::npos) continue;
                string fecha = nombre.substr(pos1, pos2 - pos1); // Ej: 22062025
                string hora = nombre.substr(pos2 + 1, nombre.size() - pos2 - 5); // Ej: 11033

                // Parsear fecha y hora
                int d, m, a, h, min, s;
                if (fecha.length() == 7) { // DMMYYYY
                    d = stoi(fecha.substr(0, 1));
                    m = stoi(fecha.substr(1, 1));
                    a = stoi(fecha.substr(2, 4));
                } else if (fecha.length() == 8) { // DDMMYYYY
                    d = stoi(fecha.substr(0, 2));
                    m = stoi(fecha.substr(2, 2));
                    a = stoi(fecha.substr(4, 4));
                } else {
                    continue;
                }
                if (hora.length() == 5) { // HMMSS
                    h = stoi(hora.substr(0, 1));
                    min = stoi(hora.substr(1, 2));
                    s = stoi(hora.substr(3, 2));
                } else if (hora.length() == 6) { // HHMMSS
                    h = stoi(hora.substr(0, 2));
                    min = stoi(hora.substr(2, 2));
                    s = stoi(hora.substr(4, 2));
                } else {
                    continue;
                }
                tm tm = {};
                tm.tm_mday = d;
                tm.tm_mon = m - 1;
                tm.tm_year = a - 1900;
                tm.tm_hour = h;
                tm.tm_min = min;
                tm.tm_sec = s;
                time_t tiempo = mktime(&tm);
                backupsTxt.emplace_back(nombre, tiempo);
            }
        }
    }
    if (backupsTxt.empty()) {
        cout << "No se encontraron archivos (.txt)." << endl;
        return;
    }

    for (const auto& backup : backupsTxt) {
        tm* tm = localtime(&backup.second);
        cout << contador << ". " << backup.first << endl;
        cout << "• Fecha: " << put_time(tm, "%d/%m/%Y") << "\n• Hora: " << put_time(tm, "%H:%M:%S") << endl << "• Tamaño: ";
        try {
            auto tamanio = fs::file_size(backup.first);
            cout << tamanio << " bytes" << endl;
        } catch (...) {
            cout << "N/A" << endl;
        }
        contador++;
    }
}

void CifradoCesar::cifrar_archivos_txt() {
    system("cls");
    cout << "\t\t===========================================" << endl;
    cout << "\t\t===    CIFRAR ARCHIVO DE RESPALDO    ===" << endl;
    cout << "\t\t===========================================" << endl;
    cout << "Presione Esc para volver al menú de administrador.\n\n";

    // 1. Buscar todos los backups disponibles (ahora .txt o según tu flujo)
    vector<string> backups;
    for (const auto& entry : filesystem::directory_iterator(".")) {
        string nombre = entry.path().filename().string();
        if (nombre.find("backup_") == 0 && nombre.substr(nombre.size() - 4) == ".txt") {
            backups.push_back(nombre);
        }
    }

    listarArchivosTxt(2);

    int seleccion = 0;
    string numero;
    do {
        ValidacionDatos::limpiar_linea("Seleccione el archivo a cifrar (1-" + to_string(backups.size()) + "): ");
        numero = ValidacionDatos::ingresar_dni("");
        if (numero == "__ESC__") return;
        if (!numero.empty()) {
            try {
                seleccion = stoi(numero);
            } catch (...) {
                seleccion = 0;
            }
        }
    } while (seleccion < 1 || seleccion > static_cast<int>(backups.size()));
    cout << endl;
    string nombre_archivo = backups[seleccion - 1];

    cout << "-------------------------------------------" << endl;
    cout << "Archivo seleccionado: " << nombre_archivo << endl;
    cout << "Iniciando proceso de cifrado..." << endl;
    int numCesar = 3;

    // Generar nombre para el archivo cifrado (igual que en guardarBackup)
    string nombre_cifrado = "cifrado_" + nombre_archivo.substr(7); // Quita "backup_"
    codificarArchivo(nombre_archivo, nombre_cifrado, numCesar);
    ifstream file(nombre_cifrado, ios::binary);
    stringstream buffer;
    buffer << file.rdbuf();
    string hash = Hash::calcularMD5(buffer.str());

    string base = nombre_cifrado.substr(8, nombre_cifrado.size() - 8 - 4); // Quita "cifrado_" y ".txt"
    string nombre_hash = "Hash_" + base + ".txt";
    ofstream hashFile(nombre_hash);
    hashFile << hash;
    hashFile.close();
    string nombre_pdf= "PDF_" + base + ".pdf";
    createPDF(nombre_cifrado, nombre_pdf);
    cout << "-------------------------------------------" << endl;
    cout << "=== ¡CIFRADO COMPLETADO CON ÉXITO! ===" << endl;
    cout << "El archivo ha sido cifrado y guardado como: " << nombre_cifrado << endl;
    cout << "===========================================" << endl;
}

void CifradoCesar::descifrar_archivos_txt() {
    system("cls");
    cout << "\t\t===========================================" << endl;
    cout << "\t\t===   DESCIFRAR ARCHIVO DE RESPALDO   ===" << endl;
    cout << "\t\t===========================================" << endl;
    cout << "Presione Esc para volver al menú de administrador.\n\n";

    // 1. Buscar todos los archivos cifrados disponibles
    vector<string> cifrados;
    for (const auto& entry : filesystem::directory_iterator(".")) {
        string nombre = entry.path().filename().string();
        if (nombre.find("cifrado_") == 0 && nombre.substr(nombre.size() - 4) == ".txt") {
            cifrados.push_back(nombre);
        }
    }
    listarArchivosTxt(1);

    int seleccion = 0;
    string numero;
    do {
        ValidacionDatos::limpiar_linea("Seleccione el archivo a descifrar (1-" + to_string(cifrados.size()) + "): ");
        numero = ValidacionDatos::ingresar_dni("");
        if (numero == "__ESC__") return;
        if (!numero.empty()) {
            try {
                seleccion = stoi(numero);
            } catch (...) {
                seleccion = 0;
            }
        }
    } while (seleccion < 1 || seleccion > static_cast<int>(cifrados.size()));
    cout << endl;
    string nombre_archivo = cifrados[seleccion - 1];

    cout << "-------------------------------------------" << endl;
    cout << "Archivo seleccionado: " << nombre_archivo << endl;
    cout << "Iniciando proceso de descifrado..." << endl;
    int numCesar = 3;

    // Generar nombre para el archivo descifrado
    string base = nombre_archivo.substr(8, nombre_archivo.size() - 8 - 4); // Quita "cifrado_" y ".txt"
    string nombre_descifrado = "descifrado_" + base + ".txt";
    decodificarArchivo(nombre_archivo, nombre_descifrado, 3);   
    cout << "-------------------------------------------" << endl;
    cout << "=== ¡DESCIFRADO COMPLETADO CON ÉXITO! ===" << endl;
    cout << "El archivo ha sido descifrado y guardado como: " << nombre_descifrado << endl;
    cout << "===========================================" << endl;
}

bool CifradoCesar::verificarIntegridadCifrados() {
    bool todoOk = true;
    for (const auto& entry : filesystem::directory_iterator(".")) {
        string nombre = entry.path().filename().string();
    if (nombre.find("cifrado_") == 0 && nombre.substr(nombre.size() - 4) == ".txt") {
        size_t pos_punto = nombre.rfind('.');
        string base = nombre.substr(8, pos_punto - 8);
        string nombre_hash = "Hash_" + base + ".txt";
        ifstream hashFile(nombre_hash);
        if (!hashFile.is_open()) {
            cout << "No se encontró el hash para: " << nombre << endl;
            todoOk = false;
            continue;
        }
        string hashGuardado;
        getline(hashFile, hashGuardado);
        hashFile.close();

        // CORRECTO: calcular hash del CONTENIDO
        ifstream file(nombre, ios::binary);
        stringstream buffer;
        buffer << file.rdbuf();
        string hashActual = Hash::calcularMD5(buffer.str());

        if (hashActual == hashGuardado) {
            cout << nombre << ": INTEGRIDAD OK" << endl;
        } else {
            cout << nombre << ": ¡INTEGRIDAD VULNERADA!" << endl;
            todoOk = false;
        }
    }
}
    return todoOk;
}

void CifradoCesar::createPDF(const string& txtFile, const string& pdfFile) {
    // Leer el archivo .txt
    ifstream inFile(txtFile);
    if (!inFile.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << txtFile << endl;
        return;
    }

    // Almacenar las líneas del archivo .txt
    vector<string> lines;
    string line;
    while (getline(inFile, line)) {
        lines.push_back(line);
    }
    inFile.close();

    // Crear el archivo .pdf
    ofstream outFile(pdfFile, ios::binary);
    if (!outFile.is_open()) {
        cerr << "Error: No se pudo crear el archivo " << pdfFile << endl;
        return;
    }

    // Escribir el encabezado del PDF
    outFile << "%PDF-1.4\n";
    outFile << "%\xE2\xE3\xCF\xD3\n"; // Comentario binario para indicar que es un archivo binario

    // Objetos del PDF
    int objCount = 1;
    std::vector<long> offsets;

    // Objeto 1: Catálogo
    offsets.push_back(outFile.tellp());
    outFile << objCount++ << " 0 obj\n";
    outFile << "<< /Type /Catalog /Pages 2 0 R >>\n";
    outFile << "endobj\n";

    // Objeto 2: Páginas
    offsets.push_back(outFile.tellp());
    outFile << objCount++ << " 0 obj\n";
    outFile << "<< /Type /Pages /Kids [3 0 R] /Count 1 >>\n";
    outFile << "endobj\n";

    // Objeto 3: Página
    offsets.push_back(outFile.tellp());
    outFile << objCount++ << " 0 obj\n";
    outFile << "<< /Type /Page /Parent 2 0 R /Resources << /Font << /F1 4 0 R >> >> /MediaBox [0 0 595 842] /Contents 5 0 R >>\n";
    outFile << "endobj\n";

    // Objeto 4: Fuente (Helvetica, estándar en PDF)
    offsets.push_back(outFile.tellp());
    outFile << objCount++ << " 0 obj\n";
    outFile << "<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>\n";
    outFile << "endobj\n";

    // Objeto 5: Contenido de la página
    offsets.push_back(outFile.tellp());
    outFile << objCount++ << " 0 obj\n";
    outFile << "<< /Length " << (lines.size() * 50 + 50) << " >>\n"; // Estimación aproximada de la longitud
    outFile << "stream\n";
    outFile << "BT\n";
    outFile << "/F1 12 Tf\n"; // Fuente Helvetica, tamaño 12
    outFile << "50 800 TD\n"; // Posición inicial (x=50, y=800)
    
    // Escribir cada línea del texto
    for (const auto& text : lines) {
        // Escapar caracteres especiales en el texto
        string escapedText = "(";
        for (char c : text) {
            if (c == '(' || c == ')' || c == '\\') {
                escapedText += '\\';
            }
            escapedText += c;
        }
        escapedText += ")";
        outFile << escapedText << " Tj\n";
        outFile << "0 -14 TD\n"; // Mover hacia abajo para la siguiente línea
    }
    
    outFile << "ET\n";
    outFile << "endstream\n";
    outFile << "endobj\n";

    // Tabla de referencias cruzadas
    long xrefOffset = outFile.tellp();
    outFile << "xref\n";
    outFile << "0 " << objCount << "\n";
    outFile << "0000000000 65535 f \n";
    for (long offset : offsets) {
        outFile << setfill('0') << setw(10) << offset << " 00000 n \n";
    }

    // Tráiler
    outFile << "trailer\n";
    outFile << "<< /Size " << objCount << " /Root 1 0 R >>\n";
    outFile << "startxref\n";
    outFile << xrefOffset << "\n";
    outFile << "%%EOF\n";

    outFile.close();
    cout << "PDF creado exitosamente: " << pdfFile << endl;
    }