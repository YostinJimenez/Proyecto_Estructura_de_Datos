#include "ValidacionDatos.h"
#include "Marquesina.h"
#include "Utilidades.h"
#include "Banco.h"
#include <regex>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <conio.h>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <set>
using namespace std;
extern Marquesina marquesina;
static bool esBisiesto(int anio) {
    return (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
}

static int diasEnMes(int mes, int anio) {
    if (mes == 4 || mes == 6 || mes == 9 || mes == 11) return 30;
    if (mes == 2) return esBisiesto(anio) ? 29 : 28;
    return 31;
}

void ValidacionDatos::limpiar_linea(const string& mensaje) {
    cout << "\r\033[K" << mensaje;
}

Fecha ValidacionDatos::capturarFechaInteractiva(const string& mensaje, bool esFechaNacimiento, const Fecha* minFecha) {
    int dia = 1, mes = 1, anio = esFechaNacimiento ? 2000 : 2025;
    if (minFecha) {
        dia = minFecha->getDia();
        mes = minFecha->getMes();
        anio = minFecha->getAnio();
    }
    int campo = 0;
    bool salir = false;

    while (!salir) {
        system("cls");
        cout << mensaje << "\n\n";
        cout << "Fecha: ";
        cout << (campo == 0 ? "[" : "") << setw(2) << setfill('0') << dia << (campo == 0 ? "]" : "");
        cout << "/";
        cout << (campo == 1 ? "[" : "") << setw(2) << setfill('0') << mes << (campo == 1 ? "]" : "");
        cout << "/";
        cout << (campo == 2 ? "[" : "") << setw(4) << anio << (campo == 2 ? "]" : "");
        cout << "\n\nUse flechas izquierda/derecha para cambiar campo, arriba/abajo para ajustar, Enter para confirmar, Esc para volver.";

        int tecla = _getch();
        if (tecla == 224 || tecla == 0) {
            tecla = _getch();
            switch (tecla) {
                case 75: campo = (campo - 1 + 3) % 3; break; // Izquierda
                case 77: campo = (campo + 1) % 3; break; // Derecha
                case 72: // Arriba
                    if (campo == 0) {
                        dia++;
                        if (dia > diasEnMes(mes, anio)) dia = 1;
                    } else if (campo == 1) {
                        mes++;
                        if (mes > 12) mes = 1;
                        if (dia > diasEnMes(mes, anio)) dia = diasEnMes(mes, anio);
                    } else {
                        anio++;
                        int maxAnio = esFechaNacimiento ? 2007 : 2100;
                        if (anio > maxAnio) anio = 1900;
                        if (dia > diasEnMes(mes, anio)) dia = diasEnMes(mes, anio);
                    }
                    break;
                case 80: { // Abajo
                    bool canDecrease = true;
                    if (minFecha) {
                        Fecha currentDate(dia, mes, anio);
                        int testDia = dia, testMes = mes, testAnio = anio;
                        if (campo == 0) {
                            if (dia - 1 < 1) {
                                testMes = mes - 1 > 0 ? mes - 1 : 12;
                                testAnio = mes - 1 == 0 ? anio - 1 : anio;
                                testDia = diasEnMes(testMes, testAnio);
                            } else {
                                testDia = dia - 1;
                            }
                        } else if (campo == 1) {
                            testMes = mes - 1 < 1 ? 12 : mes - 1;
                            testAnio = mes - 1 == 0 ? anio - 1 : anio;
                            if (dia > diasEnMes(testMes, testAnio)) {
                                testDia = diasEnMes(testMes, testAnio);
                            }
                        } else {
                            testAnio = anio - 1;
                        }
                        Fecha testDate(testDia, testMes, testAnio);
                        if (minFecha->esMayorQue(testDate)) {
                            canDecrease = false;
                        }
                    }
                    if (canDecrease) {
                        if (campo == 0) {
                            dia--;
                            if (dia < 1) dia = diasEnMes(mes, anio);
                        } else if (campo == 1) {
                            mes--;
                            if (mes < 1) mes = 12;
                            if (dia > diasEnMes(mes, anio)) dia = diasEnMes(mes, anio);
                        } else {
                            anio--;
                            int minAnio = 1900;
                            if (esFechaNacimiento) minAnio = 2007;
                            if (anio < minAnio) anio = minAnio;
                            if (dia > diasEnMes(mes, anio)) dia = diasEnMes(mes, anio);
                        }
                    }
                    break;
                }
            }
        } else if (tecla == 13) {
            try {
                Fecha fecha(dia, mes, anio);
                validarFecha(fecha, esFechaNacimiento);
                //if (!fecha.esValida()) throw BancoException("Fecha inválida.");
                return fecha;
            } catch (const BancoException& e) {
                cout << "\nError: " << e.what() << "\n";
                cout << "Presiona Enter para continuar...";
                _getch();
            }
        } else if (tecla == 27) { // Esc
            return Fecha(); // Fecha inválida (Atrás)
        }
    }
    return Fecha();
}

Fecha ValidacionDatos::capturarFechaInteractiva(const string& mensaje, bool esFechaNacimiento) {
    return capturarFechaInteractiva(mensaje, esFechaNacimiento, nullptr);
}

void ValidacionDatos::validarFecha(const Fecha& fecha, bool esFechaNacimiento) {
    int dia = fecha.getDia();
    int mes = fecha.getMes();
    int anio = fecha.getAnio();

    int maxAnio = esFechaNacimiento ? 2007 : 2100;
    if (anio < 1900 || anio > maxAnio) {
        throw BancoException("Año inválido: debe estar entre 1900 y " + std::to_string(maxAnio) + ".");
    }

    // Validar mes
    if (mes < 1 || mes > 12) {
        throw BancoException("Mes inválido: debe estar entre 1 y 12.");
    }

    // Validar día usando la lógica de es_fecha_valida
    int dias_en_mes[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (mes == 2 && ((anio % 4 == 0 && anio % 100 != 0) || anio % 400 == 0)) {
        dias_en_mes[1] = 29;
    }
    if (dia < 1 || dia > dias_en_mes[mes - 1]) {
        throw BancoException("Día inválido para el mes y año proporcionados.");
    }
    if (esFechaNacimiento) {
        Fecha fechaMinima(1, 6, 2007);
        if (fecha.esMayorQue(fechaMinima) || (fecha.getDia() == fechaMinima.getDia() && 
           fecha.getMes() == fechaMinima.getMes() && 
           fecha.getAnio() == fechaMinima.getAnio())) {
            throw BancoException("Debes ser mayor de 18 años.");
        }
    }
}

string ValidacionDatos::capturarEntrada(TipoDato tipo, const string& mensaje, size_t maxLongitud) {
    string entrada;
    int fila_entrada = marquesina.get_cursor_row(); // O pásala como parámetro si tienes control
    Utilidades::gotoxy(0, fila_entrada);
    cout << mensaje;
    cout.flush();

    while (true) {
        int tecla = _getch();
        if (tecla == 27) { // Esc
            Utilidades::gotoxy(0, fila_entrada);
            cout << "\r" << string(mensaje.length() + entrada.length(), ' ') << "\r";
            return ""; // Retorna cadena vacía para indicar "Atrás"
        }
        if (tecla == 13 && !entrada.empty()) { // Enter
            try {
                switch (tipo) {
                    case CEDULA: validarCedula(entrada); break;
                    case NOMBRE: validarNombre(entrada); break;
                    case CORREO: validarCorreo(entrada); break;
                    case TELEFONO: validarTelefono(entrada); break;
                    case USUARIO: validarUsuario(entrada); break;
                    case CONTRASENA: validarContrasena(entrada); break;
                    case FECHA: {
                        regex fechaRegex("^\\d{1,2}\\s\\d{1,2}\\s\\d{4}$|^\\d{1,2}/\\d{1,2}/\\d{4}$");
                        if (!regex_match(entrada, fechaRegex)) {
                            throw BancoException("Formato de fecha inválido. Use dd mm aaaa o dd/mm/aaaa.");
                        }
                        int d, m, a;
                        char sep;
                        istringstream iss(entrada);
                        if (entrada.find('/') != string::npos) {
                            iss >> d >> sep >> m >> sep >> a;
                        } else {
                            iss >> d >> m >> a;
                        }
                        Fecha f(d, m, a);
                        validarFecha(f, false);
                        break;
                    }
                    case MONTO: {
                        size_t pos;
                        double monto = stod(entrada, &pos);
                        if (pos != entrada.length()) {
                            throw BancoException("Monto inválido: contiene caracteres no numéricos.");
                        }
                        validarMonto(monto, false);
                        break;
                    }
                    case NOMBRE_ARCHIVO: validarNombreArchivo(entrada); break;
                    case CEDULA_PREFIJO: validarCedulaPrefijo(entrada); break;
                }
                cout << endl;
                return entrada;
            } catch (const BancoException& e) {
                Utilidades::gotoxy(40, fila_entrada);
                cout << "\r\033[K" << "Error: " << e.what(); // Limpia y muestra el error
                Utilidades::gotoxy(0 + mensaje.length() + entrada.length(), fila_entrada);
                getch(); // Espera a que el usuario vea el error
                Utilidades::gotoxy(40, fila_entrada);
                cout << "\r\033[K"; // Limpia la línea del error
                Utilidades::gotoxy(0, fila_entrada);
                cout << "\r\033[K" << mensaje; // Limpia y vuelve a mostrar el mensaje de entrada
                Utilidades::gotoxy(0 + mensaje.length(), fila_entrada);
                entrada.clear();
                cout.flush();
                continue;
            }
        }
        else if (tecla == 8 && !entrada.empty()) { // Backspace
            entrada.pop_back();
            cout << "\b \b";
            cout.flush();
        }
        else if (tecla >= 32 && tecla <= 126) { // Caracteres imprimibles
            char c = static_cast<char>(tecla);
            bool valido = false;
            switch (tipo) {
                case CEDULA:
                    if (entrada.length() < 10) {
                        valido = isdigit(c);
                    } else {
                        valido = isalnum(c);
                    }
                    break;
                case NOMBRE:
                    valido = isalpha(c) ;
                    break;
                case CORREO:
                    valido = isalnum(c) || c == '@' || c == '.' || c == '-' || c == '_';
                    break;
                case TELEFONO:
                    valido = isdigit(c);
                    break;
                case USUARIO:
                    valido = isalnum(c) || c == '_';
                    break;
                case CONTRASENA:
                    valido = isalnum(c) || c == '@' || c == '#' || c == '$' || c == '%' ||
                             c == '^' || c == '&' || c == '+' || c == '=' || c == '!';
                    break;
                case MONTO:
                    valido = isdigit(c) || (c == '.' && entrada.find('.') == string::npos);
                    break;
                case FECHA:
                    valido = isdigit(c) || c == ' ' || c == '/';
                    break;
                case NOMBRE_ARCHIVO:
                    valido = isalnum(c) || c == '.';
                    break;
                case CEDULA_PREFIJO:
                    valido = isdigit(c);
                    break;
            }
            if (valido && entrada.length() < maxLongitud) {
                entrada += c;
                if (tipo == CONTRASENA) {
                    cout << "*";
                } else {
                    cout << c;
                }
                cout.flush();
            }
        }
    }
}

void ValidacionDatos::validarCedula(const string& entrada) {
    try {
        if (entrada.length() != 10 && entrada.length() != 22) {
            throw BancoException("Debe ser cédula (10 dígitos) o IBAN (22 caracteres).");
        }
        if (entrada.length() == 22) {
            // Validación de IBAN
            if (!regex_match(entrada, regex("^EC\\d{2}30100001(22|23)\\d{8}$"))) {
                throw BancoException("Formato incorrecto. Debe ser ECXX30100001(22|23)XXXXXXXX.");
            }
            string cadena = entrada.substr(4) + "EC00";
            string numero = "";
            for (char c : cadena) {
                if (isdigit(c)) {
                    numero += c;
                } else {
                    numero += to_string(c - 'A' + 10);
                }
            }
            long long resto = 0;
            for (char c : numero) {
                resto = (resto * 10 + (c - '0')) % 97;
            }
            int codigo = 98 - resto;
            int codigoEntrada = stoi(entrada.substr(2, 2));
            if (codigo != codigoEntrada) {
                throw BancoException("Código de control incorrecto.");
            }
        } else {
            // Validación de cédula
            if (!regex_match(entrada, regex("\\d{10}"))) {
                throw BancoException("Debe proporcionar 10 dígitos numéricos.");
            }

            // Validar código de provincia
            int provincia = stoi(entrada.substr(0, 2));
            set<int> provinciasValidas = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 30};
            if (provinciasValidas.find(provincia) == provinciasValidas.end()) {
                throw BancoException("Código de provincia inválido.");
            }

            // Validar tercer dígito
            int tercerDigito = entrada[2] - '0';
            if (tercerDigito >= 6) {
                throw BancoException("Tercer dígito debe ser menor a 6.");
            }

            // Evitar dígitos todos iguales
            bool todosIguales = true;
            char primerDigito = entrada[0];
            for (char c : entrada) {
                if (c != primerDigito) {
                    todosIguales = false;
                    break;
                }
            }
            if (todosIguales) {
                throw BancoException("No se permiten dígitos todos iguales.");
            }

            // Evitar patrones repetitivos simples (e.g., 1212121212)
            bool repetitivo = true;
            for (size_t i = 0; i < entrada.length() - 5; ++i) {
                if (entrada[i] != entrada[i + 5]) {
                    repetitivo = false;
                    break;
                }
            }
            if (repetitivo) {
                throw BancoException("No se permiten patrones repetitivos.");
            }

            // Validar dígito verificador
            int suma = 0;
            for (size_t i = 0; i < 9; ++i) {
                int digito = entrada[i] - '0';
                if (i % 2 == 0) {
                    digito *= 2;
                    if (digito > 9) digito -= 9;
                }
                suma += digito;
            }
            int digitoVerificador = (10 - (suma % 10)) % 10;
            if (digitoVerificador != (entrada[9] - '0')) {
                throw BancoException("Dígito verificador incorrecto.");
            }
        }
    } catch (const regex_error& e) {
        throw BancoException("Error en la validación de cédula o IBAN.");
    }
}

void ValidacionDatos::validarCedulaPrefijo(const string& numero) {
    try {
        if (numero.empty() || numero.length() > 50) {
            throw BancoException("Debe tener entre 1 y 50 dígitos.");
        }
        if (!regex_match(numero, regex("\\d+"))) {
            throw BancoException("Solo se permiten dígitos numéricos.");
        }

        // Validar código de provincia si hay al menos 2 dígitos
        if (numero.length() >= 2) {
            int provincia = stoi(numero.substr(0, 2));
            set<int> provinciasValidas = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 30};
            if (provinciasValidas.find(provincia) == provinciasValidas.end()) {
                throw BancoException("Código de provincia inválido.");
            }
        }

        // Validar tercer dígito si hay al menos 3 dígitos
        if (numero.length() >= 3) {
            int tercerDigito = numero[2] - '0';
            if (tercerDigito >= 6) {
                throw BancoException("Tercer dígito debe ser menor a 6.");
            }
        }

        // Evitar dígitos todos iguales
        bool todosIguales = true;
        char primerDigito = numero[0];
        for (char c : numero) {
            if (c != primerDigito) {
                todosIguales = false;
                break;
            }
        }
        if (todosIguales) {
            throw BancoException("No se permiten dígitos todos iguales.");
        }

        // Evitar patrones repetitivos si hay suficientes dígitos (e.g., >= 5)
        if (numero.length() >= 5) {
            bool repetitivo = true;
            for (size_t i = 0; i < numero.length() - 4; ++i) {
                if (numero[i] != numero[i + 4]) {
                    repetitivo = false;
                    break;
                }
            }
            if (repetitivo) {
                throw BancoException("No se permiten patrones repetitivos.");
            }
        }

        // Validar dígito verificador solo si hay 10 dígitos
        if (numero.length() == 10) {
            int suma = 0;
            for (size_t i = 0; i < 9; ++i) {
                int digito = numero[i] - '0';
                if (i % 2 == 0) {
                    digito *= 2;
                    if (digito > 9) digito -= 9;
                }
                suma += digito;
            }
            int digitoVerificador = (10 - (suma % 10)) % 10;
            if (digitoVerificador != (numero[9] - '0')) {
                throw BancoException("Dígito verificador incorrecto.");
            }
        }
    } catch (const regex_error& e) {
        throw BancoException("Error en la validación de prefijo de cédula.");
    }
}

void ValidacionDatos::validarTelefono(const string& telefono) {
    try {
        // Validar formato básico: 10 dígitos comenzando con 09
        if (!regex_match(telefono, regex("^09[0-9]{8}$"))) {
            throw BancoException("Debe proporcionar 10 dígitos numéricos comenzando con 09.");
        }

        // Validar códigos de operadoras móviles válidas
        string codigoOperadora = telefono.substr(0, 3);
        set<string> codigosValidos = {"094", "096", "097", "098", "099", "085", "086", "087", "088", "089"};
        if (codigosValidos.find(codigoOperadora) == codigosValidos.end()) {
            throw BancoException("Código de operadora no válido.");
        }

        // Evitar dígitos todos iguales
        bool todosIguales = true;
        char primerDigito = telefono[0];
        for (char c : telefono) {
            if (c != primerDigito) {
                todosIguales = false;
                break;
            }
        }
        if (todosIguales) {
            throw BancoException("No se permiten dígitos todos iguales.");
        }

        // Evitar patrones repetitivos simples (e.g., 0981231231)
        bool repetitivo = true;
        for (size_t i = 0; i < telefono.length() - 4; ++i) {
            if (telefono[i] != telefono[i + 4]) {
                repetitivo = false;
                break;
            }
        }
        if (repetitivo) {
            throw BancoException("No se permiten patrones repetitivos.");
        }
    } catch (const regex_error& e) {
        throw BancoException("Error en la validación de teléfono.");
    }
}

void ValidacionDatos::validarUsuario(const string& usuario) {
    try {
        if (!regex_match(usuario, regex("^[a-zA-Z0-9._%+-]+$"))) {
            throw BancoException("Solo letras, números y ciertos caracteres permitidos.");
        }
        if (usuario.length() < 6 || usuario.length() > 20) {
            throw BancoException("Debe tener entre 6 y 20 caracteres.");
        }
    } catch (const regex_error& e) {
        throw BancoException("Error en la validación de usuario.");
    }
}

void ValidacionDatos::validarContrasena(const string& contrasenia) {
    try {
        if (contrasenia.length() < 8 || contrasenia.length() > 20) {
            throw BancoException("Debe tener entre 8 y 20 caracteres.");
        }
        if (!regex_match(contrasenia, regex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[@#$%^&+=]).+$"))) {
            throw BancoException("Debe incluir al menos una mayúscula, una minúscula, un número y un caracter especial (@#$%^&+=).");
        }
    } catch (const regex_error& e) {
        throw BancoException("Error en la validación de contraseña.");
    }
}

void ValidacionDatos::validarMonto(double monto, bool permitirCero) {
    if (!isfinite(monto)) {
        throw BancoException("Valor no numérico.");
    }
    if (!permitirCero && monto <= 0) {
        throw BancoException("El monto debe ser positivo.");
    }
    if (permitirCero && monto < 0) {
        throw BancoException("El monto no puede ser negativo.");
    }
    if (abs(monto) > 1000000.0) {
        throw BancoException("Excede el límite de $1,000,000.");
    }
    ostringstream oss;
    oss << fixed << setprecision(2) << monto;
    string montoStr = oss.str();
    size_t punto = montoStr.find('.');
    if (punto != string::npos && montoStr.length() - punto - 1 > 2) {
        throw BancoException("Máximo de 2 decimales.");
    }
}

void ValidacionDatos::validarNombreArchivo(const string& entrada) {
    try {
        if (entrada.empty() || entrada.length() > 50) {
            throw BancoException("Debe tener entre 1 y 50 caracteres.");
        }
        if (!regex_match(entrada, regex("^[a-zA-Z0-9._-]+\\.bin$"))) {
            throw BancoException("Solo letras, números, puntos, guiones y extensión .bin.");
        }
    } catch (const regex_error& e) {
        throw BancoException("Error en la validación de nombre de archivo.");
    }
}

void ValidacionDatos::validarNombre(const string& nombre) {
    try {
        if (nombre.empty() || nombre.length() > 50) {
            throw BancoException("Debe tener entre 3 y 50 caracteres.");
        }
        if (!regex_match(nombre, regex("^[a-zA-ZáéíóúÁÉÍÓÚñÑ ]+$"))) {
            throw BancoException("Solo se permiten letras y espacios.");
        }
    } catch (const regex_error& e) {
        throw BancoException("Error en la validación de nombre.");
    }
}

void ValidacionDatos::validarCorreo(const string& correo) {
    try {
        if (correo.empty() || correo.length() > 254) {
            throw BancoException("Debe tener entre 1 y 254 caracteres.");
        }
        if (!regex_match(correo, regex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$"))) {
            throw BancoException("Formato incorrecto.");
        }
    } catch (const regex_error& e) {
        throw BancoException("Error en la validacion de correo.");
    }
}

string ValidacionDatos::ingresar_dni(const string& mensaje) {
    char c;
    string palabra;
    cout << mensaje;
    while (true) {
        c = getch();
        if (c>='0'&& c<='9') {
            palabra += c;
            printf("%c", c);
        } else if (c == 13) { 
            return palabra;
        } else if (c == 8 && !palabra.empty()) { // Backspace
            palabra.pop_back();
            printf("\b \b");
        } else if (c == 27) { // ESC
            return "__ESC__"; // Valor especial para ESC
        }
    }
}

string ValidacionDatos::validarHora(const string& mensaje) {
    char buffer[7] = {0};
    char* palabra = buffer;
    char c;
    int index = 0;
    cout << mensaje;
    while (true) {
        c = getch();
        if ((c >= '0' && c <= '9') && index < 6) {
            *(palabra + index) = c;
            printf("%c", c);
            index++;
            if (index %2==0&& index < 6) {
                cout << ":";
            }
        } else if (c == 13) { // Enter
            return palabra;
        } else if (c == 8 && index > 0) { // BACKSPACE
            *(palabra + index) = '\0';
            printf("\b \b");
            if (index %2==0 && index < 6) {
                printf("\b \b");
            }
            index--;
        }else if (c == 27) { // ESC
            return "__ESC__"; // Valor especial para ESC
        }
    }
}

string ValidacionDatos::validar_Fecha(const string& mensaje) {
    char buffer[11] = {0};
    char* palabra = buffer;
    char c;
    int index = 0;
    cout << mensaje;
    while (true) {
        c = getch();
        if ((c >= '0' && c <= '9') && index < 8) {
            *(palabra + index) = c;
            printf("%c", c);
            index++;
            if (index %2==0&& index < 6) {
                cout << "/";
            }
        } else if (c == 13) { // Enter
            return palabra;
        } else if (c == 8 && index > 0) { // BACKSPACE
            *(palabra + index) = '\0';
            printf("\b \b");
            if (index %2==0 && index < 6) {
                printf("\b \b");
            }
            index--;
        }else if (c == 27) { // ESC
            return "__ESC__"; // Valor especial para ESC
        }
    }
}

bool ValidacionDatos::validar_hora_minuto_segundo(int hora, int minuto, int segundo) {
    return (hora >= 0 && hora <= 23) &&
           (minuto >= 0 && minuto <= 59) &&
           (segundo >= 0 && segundo <= 59);
}

bool ValidacionDatos::es_fecha_valida(int anio, int mes, int dia) {
    if (anio < 1900 || anio > 2100) return false;
    if (mes < 1 || mes > 12) return false;
    
    int dias_en_mes[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (mes == 2 && ((anio % 4 == 0 && anio % 100 != 0) || anio % 400 == 0)) {
        dias_en_mes[1] = 29;
    }

    if (dia < 1 || dia > dias_en_mes[mes - 1]) return false;

    return true;
}

string ValidacionDatos::validarCaracter(string& opcion) {
    opcion[0] = toupper(opcion[0]);
    for (int i = 1; i < opcion.length(); i++) {
        opcion[i] = tolower(opcion[i]);
    }
    return opcion;
}
