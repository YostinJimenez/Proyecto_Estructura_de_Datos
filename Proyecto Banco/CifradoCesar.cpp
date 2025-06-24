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
    string hash = Hash::calcularMD5(nombre_cifrado);
    string base = nombre_cifrado.substr(8, nombre_cifrado.size() - 8 - 4); // Quita "cifrado_" y ".txt"
    string nombre_hash = "Hash_" + base + ".txt";
    ofstream hashFile(nombre_hash);
    hashFile << hash;
    hashFile.close();
    cout << "-------------------------------------------" << endl;
    cout << "=== ¡CIFRADO COMPLETADO CON ÉXITO! ===" << endl;
    cout << "El archivo ha sido cifrado y guardado como: " << nombre_cifrado << endl;
    cout << "===========================================" << endl;
}

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

bool CifradoCesar::verificarIntegridad(const string& archivoCifrado) {
    string hashActual = Hash::calcularMD5(archivoCifrado);
    ifstream hashFile(archivoCifrado + ".txt");
    string hashGuardado;
    getline(hashFile, hashGuardado);
    return hashActual == hashGuardado;
}


bool CifradoCesar::verificarIntegridadCifrados() {
    bool todoOk = true;
    for (const auto& entry : filesystem::directory_iterator(".")) {
        string nombre = entry.path().filename().string();
        if (nombre.find("cifrado_") == 0 && nombre.substr(nombre.size() - 4) == ".txt") {
            // Si tienes archivos con fechas de longitud variable, mejor usa:
            size_t pos_punto = nombre.rfind('.');
            string base = nombre.substr(8, pos_punto - 8); // Quita "cifrado_" y ".txt"
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

            string hashActual = Hash::calcularMD5(nombre);
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