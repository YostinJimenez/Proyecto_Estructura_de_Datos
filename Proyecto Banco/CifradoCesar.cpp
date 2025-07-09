#include "CifradoCesar.h"
#include "Utilidades.h"
#include "PDFCreator.h"
/**
 * @brief Codifica un archivo usando el Cifrado César.
 * @param archivoEntrada Ruta del archivo de entrada a codificar.
 * @param archivoSalida Ruta del archivo de salida codificado.
 * @param clave Clave de cifrado (número de posiciones a desplazar).
 * @return true si la operación fue exitosa, false en caso contrario.
 */
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

/**
 * @brief Decodifica un archivo cifrado usando el Cifrado César.
 * Utiliza la clave negativa para revertir el cifrado.
 * @param archivoEntrada Ruta del archivo de entrada cifrado.
 * @param archivoSalida Ruta del archivo de salida descifrado.
 * @param clave Clave de cifrado utilizada originalmente.
 * @return true si la operación fue exitosa, false en caso contrario.
 */
bool CifradoCesar::decodificarArchivo(const string& archivoEntrada, const string& archivoSalida, int clave) {
    return codificarArchivo(archivoEntrada, archivoSalida, -clave);
}

/**
 * @brief Lista los archivos de respaldo en el directorio actual.
 * Permite al usuario seleccionar un archivo para cifrar o descifrar.
 * @param index 1 para cifrar, 2 para descifrar.
 */
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

/**
 * @brief Cifra un archivo de respaldo seleccionado por el usuario usando el cifrado César.
 * Genera el archivo cifrado, su hash y un PDF del contenido cifrado.
 */
void CifradoCesar::cifrar_archivos_txt() {
    system("cls");
    cout << "\n\t\t===========================================" << endl;
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
    string nombre_hash = "Hash_" + base + ".bak";
    ofstream hashFile(nombre_hash);
    hashFile << hash;
    hashFile.close();
    cout << "-------------------------------------------" << endl;
    cout << "=== ¡CIFRADO COMPLETADO CON ÉXITO! ===" << endl;
    cout << "El archivo ha sido cifrado y guardado como: " << nombre_cifrado << endl;
    cout << "===========================================" << endl;
}

/**
 * @brief Genera un PDF a partir de un archivo de respaldo cifrado.
 * Permite al usuario seleccionar el archivo y genera un PDF con su contenido.
 */
void CifradoCesar::generar_txt_a_pdf() {
    system("cls");
    cout << "\n\t\t===========================================" << endl;
    cout << "\t\t===    GENERAR PDF DE RESPALDO          ===" << endl;
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
        ValidacionDatos::limpiar_linea("Seleccione el archivo a generar PDF (1-" + to_string(backups.size()) + "): ");
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
    cout << "-------------------------------------------" << endl;

    string base = nombre_archivo.substr(8, nombre_archivo.size() - 8 - 4); // Quita "cifrado_" y ".txt"
    string carpeta = "PDFCodes";
    CreateDirectoryA(carpeta.c_str(), NULL);

    string nombre_pdf = carpeta + "\\PDF_" + base + ".pdf"; // <-- Ahora sí se guarda en la carpeta
    PDFCreator pdf;
    pdf.createPDF(nombre_archivo, nombre_pdf);
    cout << "-------------------------------------------" << endl;
    cout << "=== ¡PDF GENERADO CON ÉXITO! ===" << endl;
    cout << "El archivo ha sido generado y guardado como: " << nombre_pdf << endl;
    cout << "===========================================" << endl;
}

/**
 * @brief Descifra un archivo cifrado seleccionado por el usuario usando el cifrado César.
 * Genera el archivo descifrado y muestra el resultado.
 */
void CifradoCesar::descifrar_archivos_txt() {
    system("cls");
    cout << "\n\t\t===========================================" << endl;
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

/**
 * @brief Verifica la integridad de los archivos cifrados comparando su hash actual con el guardado.
 * @return true si todos los archivos tienen integridad OK, false si alguno falla.
 */
bool CifradoCesar::verificarIntegridadCifrados() {
    bool todoOk = true;
    for (const auto& entry : filesystem::directory_iterator(".")) {
        string nombre = entry.path().filename().string();
    if (nombre.find("cifrado_") == 0 && nombre.substr(nombre.size() - 4) == ".txt") {
        size_t pos_punto = nombre.rfind('.');
        string base = nombre.substr(8, pos_punto - 8);
        string nombre_hash = "Hash_" + base + ".bak";
        ifstream hashFile(nombre_hash);
        if (!hashFile.is_open()) {
            cout << "No se encontró el hash para: " << nombre << endl;
            todoOk = false;
            continue;
        }
        string hashGuardado;
        getline(hashFile, hashGuardado);
        hashFile.close();

        ifstream file(nombre, ios::binary);
        stringstream buffer;
        buffer << file.rdbuf();
        string hashActual,fecha,hora;
        hashActual = Hash::calcularMD5(buffer.str());
    if (hashActual == hashGuardado) {
        fecha = base.substr(0, 2) + "/" + base.substr(2, 2) + "/" + base.substr(4, 4);
        hora = base.substr(8, 2) + ":" + base.substr(10, 2) + ":" + base.substr(12, 2);
        cout << "\n╔══════════════════════════════════════════════════════╗\n";
        cout << "║              VERIFICACIÓN DE INTEGRIDAD              ║\n";
        cout << "╠══════════════════════════════════════════════════════╣\n";
        cout << "║ Archivo cifrado : " << setw(35) << left << nombre << "║\n";
        cout << "║ Archivo hash    : " << setw(35) << left << nombre_hash << "║\n";
        cout << "║ Hash guardado   : " << setw(35) << left << hashGuardado << "║\n";
        cout << "║ Fecha           : " << setw(35) << left << fecha << "║\n";
        cout << "║ Hora            : " << setw(35) << left << hora << "║\n";
        cout << "╠══════════════════════════════════════════════════════╣\n";
        cout << "║              INTEGRIDAD: \033[32mOK\033[0m                          ║\n";
        cout << "╚══════════════════════════════════════════════════════╝\n";
    } else {
        fecha = base.substr(0, 2) + "/" + base.substr(2, 2) + "/" + base.substr(4, 4);
        hora = base.substr(8, 2) + ":" + base.substr(10, 2) + ":" + base.substr(12, 2);
        cout << "\n╔══════════════════════════════════════════════════════╗\n";
        cout << "║              VERIFICACIÓN DE INTEGRIDAD              ║\n";
        cout << "╠══════════════════════════════════════════════════════╣\n";
        cout << "║ Archivo cifrado : " << setw(35) << left << nombre << "║\n";
        cout << "║ Archivo hash    : " << setw(35) << left << nombre_hash << "║\n";
        cout << "║ Hash guardado   : " << setw(35) << left << hashGuardado << "║\n";
        cout << "║ Fecha           : " << setw(35) << left << fecha << "║\n";
        cout << "║ Hora            : " << setw(35) << left << hora << "║\n";
        cout << "╠══════════════════════════════════════════════════════╣\n";
        cout << "║        INTEGRIDAD: \033[31mVULNERADA\033[0m                        ║\n";
        cout << "╚══════════════════════════════════════════════════════╝\n";
        todoOk = false;
    }
    }
}
    return todoOk;
}
