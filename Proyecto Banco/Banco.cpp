#define WIN32_LEAN_AND_MEAN
#include "Banco.h"
#include "GeneradorQr.h"
#include "PDFCreator.h"
#include "Utilidades.h"
#include "ValidacionDatos.h"
#include "CuentaAhorros.h"
#include "CuentaCorriente.h"
#include "recuperarCuenta.h"
#include "ListaDobleCircular.h"
#include "BackupManager.h"
#include <iostream>
#include <string>
#include <limits>
#include <conio.h>
#include <shellapi.h>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include "Marquesina.h"
using namespace std;
/** @brief Puntero global a la marquesina utilizada en la aplicación */
Marquesina* marquesinaGlobal=nullptr;

/** @brief Instancia global de la marquesina para mostrar mensajes en la consola */
Marquesina marquesina(0, 0, 200, "marquesina.html", 50);

/** @brief Bandera atómica para coordinar actualizaciones entre menú y marquesina */
std::atomic<bool> actualizandoMenu(false);

/** @brief Mutex para sincronización entre hilos */
std::mutex mtxActualizacion;

/**
 * @brief Calcula el código de control para un IBAN ecuatoriano.
 * @param ibanSinControl IBAN sin el código de control.
 * @return Código de control de dos dígitos como string.
 */
static string calcularCodigoControl(const string& ibanSinControl) {
    string cadena = ibanSinControl.substr(4) + "EC00";
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
    ostringstream oss;
    oss << setw(2) << setfill('0') << codigo;
    return oss.str();
}

/**
 * @brief Constructor de la clase Banco.
 * Inicializa la lista de clientes y otros recursos.
 */
Banco::Banco() {
    // Inicialización por defecto de clientes (ListaDobleCircular ya se inicializa sola)
}

/**
 * @brief Muestra el manual de ayuda técnica del banco.
 * Abre el archivo de ayuda AyudaTecnicaBanco.chm.
 */
void Banco::mostrar_ayuda_tecnica()
{
    system("cls");
    try
    {
        cout << "\n\t\t===========================================" << endl;
        cout << "\t\t===    MANUAL DE AYUDA TÉCNICA BANCO    ===" << endl;
        cout << "\t\t===========================================" << endl;
        HINSTANCE result = ShellExecute(NULL, "open", "AyudaTecnicaBanco.chm", NULL, NULL, SW_SHOWNORMAL);
        if (result <= (HINSTANCE)32)
        {
            throw runtime_error("No se pudo abrir el archivo de ayuda. Asegúrese de que 'AyudaTecnicaBanco.chm' esté en el directorio del programa.");
        }
        cout << "Manual de Ayuda Técnica abierto exitosamente.\n";
        getch(); // Esperar a que el usuario vea el mensaje
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        cout << "\n=== ERROR AL ABRIR AYUDA TÉCNICA ===" << endl;
        cout << "Error: " << e.what() << endl;
        cout << "\nRegresando al menú principal...\n";
    }
}

/**
 * @brief Genera la documentación del proyecto usando Doxygen y la abre en el navegador.
 */
void Banco::generarDocumentacion() {
    system("cls");
    cout << "\n\t\t=====================================\n";
    cout << "\t\t=== GENERANDO DOCUMENTACIÓN... ====\n";
    cout << "\t\t=====================================\n";
    cout <<"Generando documentación del proyecto Banco...\n";
    ShellExecute(NULL, "open", "documentacion\\html\\index.html", NULL, NULL, SW_SHOWNORMAL);
    getch(); // Esperar a que el usuario vea el mensaje
}

/**
 * @brief Permite seleccionar una opción de un menú usando el teclado.
 * @param titulo Título del menú.
 * @param opciones Arreglo de strings con las opciones.
 * @param n Número de opciones.
 * @param fila_inicio Fila inicial para mostrar el menú.
 * @return Índice de la opción seleccionada (1-based).
 */
int Banco::seleccionar_opcion(const char *titulo, const char *opciones[], int n, int fila_inicio) {
    system("chcp 65001 > nul");
    int opcionSeleccionada = 1;
    int tecla;

    std::lock_guard<std::mutex> lock(mtxActualizacion);

    Utilidades::gotoxy(1, fila_inicio);
    cout << titulo;

    // Dibujar las opciones iniciales, empezando justo debajo del título
    for (int e = 0; e < n; e++) {
        Utilidades::gotoxy(10, fila_inicio + 2 + e); // +2 para dejar un espacio entre título y opciones
        if (e + 1 == opcionSeleccionada) {
            cout << " ➤ " << e + 1 << " " << opciones[e];
        } else {
            cout << "    " << e + 1 << " " << opciones[e];
        }
    }

    do {
        tecla = getch();
        switch (tecla) {
            case TECLA_ARRIBA:
                // Actualizar la opción seleccionada
                Utilidades::gotoxy(10, fila_inicio + 2 + (opcionSeleccionada - 1));
                cout << "    " << opcionSeleccionada << " " << opciones[opcionSeleccionada - 1];

                opcionSeleccionada--;
                if (opcionSeleccionada < 1) opcionSeleccionada = n;

                Utilidades::gotoxy(10, fila_inicio + 2 + (opcionSeleccionada - 1));
                cout << " ➤ " << opcionSeleccionada << " " << opciones[opcionSeleccionada - 1];
                break;

            case TECLA_ABAJO:
                // Actualizar la opción seleccionada
                Utilidades::gotoxy(10, fila_inicio + 2 + (opcionSeleccionada - 1));
                cout << "    " << opcionSeleccionada << " " << opciones[opcionSeleccionada - 1];

                opcionSeleccionada++;
                if (opcionSeleccionada > n) opcionSeleccionada = 1;

                Utilidades::gotoxy(10, fila_inicio + 2 + (opcionSeleccionada - 1));
                cout << " ➤ " << opcionSeleccionada << " " << opciones[opcionSeleccionada - 1];
                break;
            }
    } while (tecla != ENTER);
    Sleep(2);
	actualizandoMenu = false;

    return opcionSeleccionada;
}

/**
 * @brief Pausa temporalmente la marquesina durante operaciones críticas
 */
static void pausarMarquesina() {
	if (marquesinaGlobal) {
		marquesinaGlobal->pausar();
	}
}

/**
 * @brief Reanuda la marquesina después de una operación crítica
 */
static void reanudarMarquesina() {
	if (marquesinaGlobal) {
		marquesinaGlobal->reanudar();
	}
}

/**
 * @brief Detiene la marquesina global si está activa.
 */
static void detenerMarquesina() {
	if (marquesinaGlobal) {
		marquesinaGlobal->detener();
	}
}

/**
 * @brief Muestra un cuadro de confirmación para operaciones monetarias.
 * @param monto Monto de la operación.
 * @param operacion Nombre de la operación (ej: "TRANSFERENCIA").
 * @return true si el usuario acepta, false si cancela.
 */
bool Banco::mostrarConfirmacion(double monto, const string& operacion) {
    system("cls");
    const char* opciones[] = {"Aceptar", "Cancelar"};
    cout <<"\n\t=====================================\n";
    cout << "\t======= CONFIRMAR " << operacion << " =======\n";
    cout <<"\t=====================================\n";

    cout << "\tMonto: $" << fixed << setprecision(2) << monto << "\n";
    cout << "\t¿Está seguro?\n\n";
    pausarMarquesina(); // Pausar marquesina antes de mostrar confirmación
    int seleccion = seleccionar_opcion("", opciones, 5, 3);
    reanudarMarquesina(); // Reanudar marquesina después de la confirmación
    if (seleccion == 2) return false; // Atrás
    return (seleccion == 1); // Aceptar (true), Cancelar (false)
}

/**
 * @brief Permite al usuario seleccionar el tipo de cuenta (Ahorros/Corriente).
 * @return Tipo de cuenta seleccionado o vacío si cancela.
 */
string Banco::seleccionarTipoCuenta() {
    system("cls");
    const char* opciones[] = {"Ahorros", "Corriente", "Atrás"};
    pausarMarquesina(); // Pausar marquesina antes de mostrar opciones
    int seleccion = seleccionar_opcion("\t\t=== SELECCIONE TIPO DE CUENTA ===\n\n", opciones, 3, 2);
    reanudarMarquesina(); // Reanudar marquesina después de la selección
    if (seleccion == 3) return ""; // Atrás
    return opciones[seleccion - 1];
}

/**
 * @brief Crea una nueva cuenta bancaria y la agrega a la lista de clientes.
 */
void Banco::crearCuenta() {
    system("cls");
    cout <<"\n\t_________________________________\n";
    cout << "\t||    CREACION DE CUENTA       ||\n";
    cout << "\t||_____________________________||\n";
    cout << "• Presione Esc para volver al menú principal.\n\n";
    string primerNombre, segundoNombre, primerApellido, segundoApellido, cedula, telefono, correo, tipoCuenta, usuario, contrasena, repetirContrasena;
    Fecha fechaNacimiento;
    try {
        primerNombre = ValidacionDatos::capturarEntrada(ValidacionDatos::NOMBRE, "Primer nombre: ", 50);
        if (primerNombre.empty()) return; // Atrás
        ValidacionDatos::validarCaracter(primerNombre);
        segundoNombre = ValidacionDatos::capturarEntrada(ValidacionDatos::NOMBRE, "Segundo nombre: ", 50);
        if (segundoNombre.empty()) return;
        ValidacionDatos::validarCaracter(segundoNombre);
        primerApellido = ValidacionDatos::capturarEntrada(ValidacionDatos::NOMBRE, "Primer apellido: ", 50);
        if (primerApellido.empty()) return;
        ValidacionDatos::validarCaracter(primerApellido);
        segundoApellido = ValidacionDatos::capturarEntrada(ValidacionDatos::NOMBRE, "Segundo apellido: ", 50);
        if (segundoApellido.empty()) return;
        ValidacionDatos::validarCaracter(segundoApellido);
        cedula = ValidacionDatos::capturarEntrada(ValidacionDatos::CEDULA, "Cédula: ", 10);
        if (cedula.empty()) return;
        // Verificar unicidad de cédula
        Cliente* existeCedula = clientes.buscar([&](Cliente* c) { return c->getCedula() == cedula; });
        if (existeCedula) throw BancoException("Ya existe un cliente con esta cédula.");
        telefono = ValidacionDatos::capturarEntrada(ValidacionDatos::TELEFONO, "Teléfono celular: ", 10);
        if (telefono.empty()) return;
        correo = ValidacionDatos::capturarEntrada(ValidacionDatos::CORREO, "Correo: ", 254);
        if (correo.empty()) return;
        fechaNacimiento = ValidacionDatos::capturarFechaInteractiva("Fecha de nacimiento (dd/mm/aaaa): ", true);
        if (!fechaNacimiento.esValida()) return; // Atrás
        tipoCuenta = seleccionarTipoCuenta();
        if (tipoCuenta.empty()) return;
        system("cls");
        cout <<"\n\t_________________________________\n";
        cout << "\t||    CREACION DE CUENTA       ||\n";
        cout << "\t||_____________________________||\n";
        cout << "• Presione Esc para volver al menú principal.\n\n";
        usuario = ValidacionDatos::capturarEntrada(ValidacionDatos::USUARIO, "Usuario: ", 20);
        if (usuario.empty()) return;
        Cliente* existeUsuario = clientes.buscar([&](Cliente* c) { return c->getUsuario() == usuario; });
        if (existeUsuario) throw BancoException("El usuario ya existe.");
        contrasena = ValidacionDatos::capturarEntrada(ValidacionDatos::CONTRASENA, "Contrasena: ", 20);
        if (contrasena.empty()) return;
        repetirContrasena = ValidacionDatos::capturarEntrada(ValidacionDatos::CONTRASENA, "Repetir contrasena: ", 20);
        if (repetirContrasena.empty()) return;
        if (contrasena != repetirContrasena) throw BancoException("Las contraseñas no coinciden.");

        string numCuenta = generarNumeroCuenta(tipoCuenta);
        Cliente* cliente = new Cliente(cedula, primerNombre, segundoNombre, primerApellido, segundoApellido,
                                       telefono, "Direccion", fechaNacimiento, "COD" + cedula, correo, usuario, contrasena);
        Cuenta* cuenta;
        if (tipoCuenta == "Ahorros") {
            cuenta = new CuentaAhorros(numCuenta, cliente, Fecha(), 0.05, 5);
        } else {
            cuenta = new CuentaCorriente(numCuenta, cliente, Fecha(), 1000.0);
        }
        cliente->setCuenta(cuenta);
        clientes.insertar(cliente);
        cout << "\nCuenta creada exitosamente para " << primerNombre << " " << segundoNombre << " " << primerApellido << " " << segundoApellido << ".\n";
        cout << "Usuario: " << usuario << "\n";
        cout << "Número de cuenta: " << numCuenta << "\n";
    } catch (const BancoException& e) {
        cout << "\nError: " << e.what() << "\n";
    }
    cout << "\nPresiona Enter para volver...";
    getch();
}

/**
 * @brief Imprime un cuadro de búsqueda con el texto centrado.
 * @param texto Texto a mostrar en el cuadro.
 * @param ancho Ancho del cuadro (por defecto 46).
 */
void Banco::imprimirCuadroBusqueda(const std::string& texto, int ancho = 46) {
    std::string titulo = "BÚSQUEDA POR " + texto;
    int espacios = ancho - 4 - titulo.size(); // 4 por los bordes y espacios
    int espaciosIzq = espacios / 2;
    int espaciosDer = espacios - espaciosIzq;
    std::cout << "\n\t\t";
    for (int i = 0; i < ancho; ++i) std::cout << "_";
    std::cout << "\n\t\t||" << std::string(espaciosIzq + 1, ' ') << titulo << std::string(espaciosDer + 1, ' ') << "||\n";
    std::cout << "\t\t||";
    for (int i = 0; i < ancho - 4; ++i) std::cout << "_";
    std::cout << "||\n";
}

 /**
 * @brief Muestra el menú de usuario para iniciar sesión y operar sobre su cuenta.
 */
void Banco::mostrarMenuUsuario() {
    system("cls");
    try {
        cout<<"\n\t=====================================\n";
        cout << "\t======= INICIO DE SESION ===========\n";
        cout << "\t=====================================\n";
        cout << "Presione Esc para volver al menú principal.\n\n";
        string usuario = ValidacionDatos::capturarEntrada(ValidacionDatos::USUARIO, "Usuario: ", 20);
        if (usuario.empty()) {
            return; // Atrás
        }
        string contrasena = ValidacionDatos::capturarEntrada(ValidacionDatos::CONTRASENA, "Contrasena: ", 20);
        if (contrasena.empty()) {
            return;
        }
        Cliente* cliente = clientes.buscar([&](Cliente* c) {
            return c->getUsuario() == usuario && c->getContrasena() == contrasena;
        });
        if (!cliente) throw BancoException("USUARIO O CONTRASENA INCORRECTOS.");
        mostrarMenuUsuario(cliente);
    } catch (const BancoException& e) {
        cout << "\n================================================\n";
        cout << "=== ERROR: " << e.what() << " ===\n";
        cout << "================================================\n";
        cout << "\nPresiona Enter para volver...";
        getch();
    }
}

/**
 * @brief Muestra el menú de usuario autenticado y permite realizar operaciones.
 * @param cliente Puntero al cliente autenticado.
 */
void Banco::mostrarMenuUsuario(Cliente* cliente) {
    const char* opciones[] = {"Transferir", "Depositar", "Retirar", "Consultar Transacciones", "Atrás"};
    bool salir = false;
    while (!salir) {
        system("cls");
        cout << "\n\n\t\t=== MENU USUARIO ===\n\n";
        Cuenta* cuenta = cliente->getCuenta();
        string tipoCuenta = dynamic_cast<CuentaAhorros*>(cuenta) ? "Ahorros" : "Corriente";
        cout << "\tNúmero de cuenta: " << cuenta->getNumero() << "\n";
        cout << "\tTipo de cuenta: " << tipoCuenta << "\n";
        cout << "\tSaldo: $" << fixed << setprecision(2) << cuenta->getSaldo() << "\n\n";
        pausarMarquesina(); // Pausar marquesina antes de mostrar opciones
        int seleccion = seleccionar_opcion("", opciones, 5, 6);
        reanudarMarquesina(); // Reanudar marquesina después de la selección
        if (seleccion == 5) {
            salir = true;
            continue; // Atrás
        }
        system("cls");
        try {
            switch (seleccion) {
                case 1: { // Transferir
                    cout << "\n=== REALIZAR TRANSFERENCIA ===\n\n";
                    cout << "Presione Esc para volver al menú de usuario.\n\n";
                    string cuentaDestino = ValidacionDatos::capturarEntrada(ValidacionDatos::CEDULA, "Número de cuenta bancaria: ", 22);
                    if (cuentaDestino.empty()) break; // Atrás
                    if (cuentaDestino == cliente->getCuenta()->getNumero()) {
                        throw BancoException("No se puede transferir a la misma cuenta.");
                    }
                    string inputMonto = ValidacionDatos::capturarEntrada(ValidacionDatos::MONTO, "Monto: $", 20);
                    if (inputMonto.empty()) break;
                    double monto = stod(inputMonto);
                    Cliente* destino = clientes.buscar([&](Cliente* c) { return c->getCuenta()->getNumero() == cuentaDestino; });
                    if (!destino) throw BancoException("Cuenta no encontrada.");
                    if (mostrarConfirmacion(monto, "TRANSFERENCIA")) {
                        if (cliente->getCuenta()->retirar(monto, Fecha(), true, cuentaDestino)) {
                            destino->getCuenta()->depositar(monto, Fecha(), true, cliente->getCuenta()->getNumero());
                            cout << "\n\n\tTransferencia de $" << fixed << setprecision(2) << monto << " realizada con éxito.\n";
                        }
                    } else {

                        cout << "\nTransferencia cancelada o abortada.\n";
                    }
                    break;
                }
                case 2: { // Depositar
                    cout << "\n=== REALIZAR DEPOSITO ===\n\n";
                    cout << "Presione Esc para volver al menú de usuario.\n\n";
                    bool depositoRealizado = false;
                    while (!depositoRealizado) {
                        string inputMonto = ValidacionDatos::capturarEntrada(ValidacionDatos::MONTO, "Monto: $", 20);
                        if (inputMonto.empty()) break; // Atrás
                        double monto = stod(inputMonto);
                        if (mostrarConfirmacion(monto, "DEPOSITO")) {
                            cliente->getCuenta()->depositar(monto);
                            cout << "\nDepósito de $" << fixed << setprecision(2) << monto << " realizado con éxito.\n";
                            depositoRealizado = true;
                        } else {
                            cout << "\nDepósito cancelado. Ingrese un nuevo monto o presione Esc para volver.\n";
                            cout << "Presiona Enter para continuar...";
                            _getch();
                            system("cls");
                            cout << "=== REALIZAR DEPOSITO ===\n\n";
                            cout << "Presione Esc para volver al menú de usuario.\n\n";
                        }
                    }
                    break;
                }
                case 3: { // Retirar
                    cout << "\n=== REALIZAR RETIRO ===\n\n";
                    cout << "Presione Esc para volver al menú de usuario.\n\n";
                    bool retiroRealizado = false;
                    while (!retiroRealizado) {
                        string inputMonto = ValidacionDatos::capturarEntrada(ValidacionDatos::MONTO, "Monto: $", 20);
                        if (inputMonto.empty()) break; // Atrás
                        double monto = stod(inputMonto);
                        if (mostrarConfirmacion(monto, "RETIRO")) {
                            if (cliente->getCuenta()->retirar(monto)) {
                                cout << "\nRetiro de $" << fixed << setprecision(2) << monto << " realizado con éxito.\n";
                                retiroRealizado = true;
                            }
                        } else {
                            cout << "\nRetiro cancelado. Ingrese un nuevo monto o presione Esc para volver.\n";
                            cout << "Presiona Enter para continuar...";
                            _getch();
                            system("cls");
                            cout << "=== REALIZAR RETIRO ===\n\n";
                            cout << "Presione Esc para volver al menú de usuario.\n\n";
                        }
                    }
                    break;
                }
                case 4: { // Consultar Transacciones
                int seleccion = 0;
                do {
                    system("cls");
                    const char* opciones_Transacciones[] = {"Rango de Fechas", "Monto Mínimo", "General", "Atrás"};
                    pausarMarquesina();
                    seleccion = seleccionar_opcion("\t\t=== CONSULTAR TRANSACCIONES ===\n\n", opciones_Transacciones, 4, 2);
                    reanudarMarquesina();
                    string tituloConsulta = "\n\t=== CONSULTAR TRANSACCIONES ===\n\nPresione Esc para volver al menú de usuario.\n\n";
                    switch (seleccion) {
                        case 1: { // Rango de Fechas
                            system("cls");
                            Fecha inicio = ValidacionDatos::capturarFechaInteractiva("Ingrese fecha de inicio (dd/mm/aaaa):");
                            if (!inicio.esValida()) break; // Atrás
                            Fecha fin = ValidacionDatos::capturarFechaInteractiva("Fecha de fin (dd/mm/aaaa):", false, &inicio);
                            cout << "\n\n";
                            if (!fin.esValida()) break;
                            cliente->getCuenta()->consultaRangoFechas(inicio, fin, [](const Transaccion<double>* t) {
                                cout << t->resumenTransaccion() << endl;
                            });
                            getch(); // Esperar a que el usuario vea las transacciones
                            break;
                        }
                        case 2: { // Monto Mínimo
                            system("cls");
                            cout << tituloConsulta;
                            string inputMonto = ValidacionDatos::capturarEntrada(ValidacionDatos::MONTO, "Monto mínimo: $", 20);
                            if (inputMonto.empty()) break;
                            double minMonto = stod(inputMonto);
                            cliente->getCuenta()->consultaPersonalizada(minMonto, [](const Transaccion<double>* t) {
                                cout << t->resumenTransaccion() << endl;
                            });
                            getch(); // Esperar a que el usuario vea las transacciones
                            break;
                        }
                        case 3: { // General
                            system("cls");
                            cout << tituloConsulta;
                            cliente->getCuenta()->consultaGeneral([](const Transaccion<double>* t) {
                                cout << t->resumenTransaccion() << endl;
                            });
                            getch(); // Esperar a que el usuario vea las transacciones
                            break;
                        }
                        case 4: // Atrás
                            break;
                    }
                } while (seleccion != 4);
                break;
            }
            }
        } catch (const BancoException& e) {
            cout << "\nError: " << e.what() << endl;
        } catch (const std::invalid_argument&) {
            cout << "\nError: Entrada inválida, ingrese solo números.\n";
        } catch (const std::out_of_range&) {
            cout << "\nError: Entrada excede el rango permitido.\n";
        }
    }
}

/**
 * @brief Busca clientes cuyo campo comienza con un prefijo dado.
 * @param campo Campo a buscar (ej: CEDULA, PRIMER_NOMBRE).
 * @param valor Prefijo a buscar.
 * @param resultados Vector donde se almacenan los clientes encontrados.
 */
void Banco::buscarPorPrefijo(CampoBusqueda campo, const string& valor, vector<Cliente*>& resultados) {
    // Extraer clientes a un vector
    vector<Cliente*> clientesVector;
    clientes.recorrer([&](Cliente* c) {
        clientesVector.push_back(c);
    });

    // Ordenar según el campo
    sort(clientesVector.begin(), clientesVector.end(), [&](Cliente* a, Cliente* b) {
        switch (campo) {
            case CEDULA: return a->getCedula() < b->getCedula();
            case PRIMER_NOMBRE: return a->getPrimerNombre() < b->getPrimerNombre();
            case SEGUNDO_NOMBRE: return a->getSegundoNombre() < b->getSegundoNombre();
            case PRIMER_APELLIDO: return a->getPrimerApellido() < b->getPrimerApellido();
            case SEGUNDO_APELLIDO: return a->getSegundoApellido() < b->getSegundoApellido();
            default: return false;
        }
    });

    // Búsqueda binaria para encontrar el primer elemento con el prefijo
    int izquierda = 0, derecha = clientesVector.size() - 1;
    int inicio = -1;
    while (izquierda <= derecha) {
        int medio = izquierda + (derecha - izquierda) / 2;
        string valorMedio;
        switch (campo) {
            case CEDULA: valorMedio = clientesVector[medio]->getCedula(); break;
            case PRIMER_NOMBRE: valorMedio = clientesVector[medio]->getPrimerNombre(); break;
            case SEGUNDO_NOMBRE: valorMedio = clientesVector[medio]->getSegundoNombre(); break;
            case PRIMER_APELLIDO: valorMedio = clientesVector[medio]->getPrimerApellido(); break;
            case SEGUNDO_APELLIDO: valorMedio = clientesVector[medio]->getSegundoApellido(); break;
            default: break;
        }
        if (valorMedio.substr(0, valor.length()) >= valor) {
            inicio = medio;
            derecha = medio - 1;
        } else {
            izquierda = medio + 1;
        }
    }

    // Recolectar todos los elementos con el prefijo
    if (inicio != -1) {
        for (int i = inicio; i < clientesVector.size(); ++i) {
            string valorActual;
            switch (campo) {
                case CEDULA: valorActual = clientesVector[i]->getCedula(); break;
                case PRIMER_NOMBRE: valorActual = clientesVector[i]->getPrimerNombre(); break;
                case SEGUNDO_NOMBRE: valorActual = clientesVector[i]->getSegundoNombre(); break;
                case PRIMER_APELLIDO: valorActual = clientesVector[i]->getPrimerApellido(); break;
                case SEGUNDO_APELLIDO: valorActual = clientesVector[i]->getSegundoApellido(); break;
                default: break;
            }
            if (valorActual.substr(0, valor.length()) == valor) {
                resultados.push_back(clientesVector[i]);
            } else {
                break;
            }
        }
    }
}

/**
 * @brief Busca clientes cuyo saldo está dentro de un rango ±10% del valor dado.
 * @param valor Saldo base para el rango.
 * @param resultados Vector donde se almacenan los clientes encontrados.
 */
void Banco::buscarPorRangoSaldo(double valor, vector<Cliente*>& resultados) {
    // Extraer clientes a un vector
    vector<Cliente*> clientesVector;
    clientes.recorrer([&](Cliente* c) {
        clientesVector.push_back(c);
    });

    // Ordenar por saldo
    sort(clientesVector.begin(), clientesVector.end(), 
         [](Cliente* a, Cliente* b) { return a->getCuenta()->getSaldo() < b->getCuenta()->getSaldo(); });

    // Búsqueda binaria para encontrar el primer cliente con saldo >= min
    double min = valor * 0.9, max = valor * 1.1; // ±10%
    int izquierda = 0, derecha = clientesVector.size() - 1;
    int inicio = -1;
    while (izquierda <= derecha) {
        int medio = izquierda + (derecha - izquierda) / 2;
        double saldoMedio = clientesVector[medio]->getCuenta()->getSaldo();
        if (saldoMedio >= min) {
            inicio = medio;
            derecha = medio - 1;
        } else {
            izquierda = medio + 1;
        }
    }

    // Recolectar clientes dentro del rango
    if (inicio != -1) {
        for (int i = inicio; i < clientesVector.size() && clientesVector[i]->getCuenta()->getSaldo() <= max; ++i) {
            resultados.push_back(clientesVector[i]);
        }
    }
}

/**
 * @brief Ordena y muestra una lista de clientes según el campo especificado.
 * @param resultados Vector de clientes a ordenar.
 * @param campo Campo por el que se ordena.
 */
void Banco::ordenarCuentas(const vector<Cliente*>& resultados, CampoBusqueda campo) {
    vector<Cliente*> clientesOrdenados = resultados; // Copia para ordenar

    // Ordenamiento por inserción
    for (size_t i = 1; i < clientesOrdenados.size(); ++i) {
        Cliente* clave = clientesOrdenados[i];
        int j = i - 1;
        while (j >= 0) {
            bool mover = false;
            switch (campo) {
                case CEDULA:
                    mover = clientesOrdenados[j]->getCedula() > clave->getCedula();
                    break;
                case PRIMER_NOMBRE:
                    mover = clientesOrdenados[j]->getPrimerNombre() > clave->getPrimerNombre();
                    break;
                case SEGUNDO_NOMBRE:
                    mover = clientesOrdenados[j]->getSegundoNombre() > clave->getSegundoNombre();
                    break;
                case PRIMER_APELLIDO:
                    mover = clientesOrdenados[j]->getPrimerApellido() > clave->getPrimerApellido();
                    break;
                case SEGUNDO_APELLIDO:
                    mover = clientesOrdenados[j]->getSegundoApellido() > clave->getSegundoApellido();
                    break;
                case SALDO:
                    mover = clientesOrdenados[j]->getCuenta()->getSaldo() > clave->getCuenta()->getSaldo();
                    break;
            }
            if (mover) {
                clientesOrdenados[j + 1] = clientesOrdenados[j];
                j--;
            } else {
                break;
            }
        }
        clientesOrdenados[j + 1] = clave;
    }

    // Mostrar resultados
    cout << setfill(' ') << setw(15) << left << "Cédula" << setw(15) << "Primer Nombre" << setw(15) << "Segundo Nombre"
         << setw(15) << "Primer Apellido" << setw(15) << "Segundo Apellido" << setw(10) << "Saldo" << endl;
    for (const auto& cliente : clientesOrdenados) {
        cout << setfill(' ') << setw(15) << left << cliente->getCedula()
             << setw(15) << cliente->getPrimerNombre()
             << setw(15) << cliente->getSegundoNombre()
             << setw(15) << cliente->getPrimerApellido()
             << setw(15) << cliente->getSegundoApellido()
             << setw(10) << fixed << setprecision(2) << cliente->getCuenta()->getSaldo() << endl;
    }
}

/**
 * @brief Permite seleccionar un cliente y generar su código QR y PDF asociado.
 */
void Banco::generarQR() {
    while (true) {
        system("cls");
        if (clientes.estaVacia()) {
            cout << "\n\t\t===================================\n";
            cout << "\t\t=== NO HAY CLIENTES REGISTRADOS ===\n";
            cout << "\t\t===================================\n";
            cout << "Presiona Enter para volver...";
            getch();
            return;
        }
        // Mostrar lista de clientes numerada
        vector<Cliente*> listaClientes;
        clientes.recorrer([&](Cliente* c) { listaClientes.push_back(c); });
        cout << "\n\t\t===============================================\n";
        cout << "\t\t==== SELECCIONE UN CLIENTE PARA GENERAR QR ====\n";
        cout << "\t\t===============================================\n\n";
        cout << left<< "| " << setw(4)  << "#" << "| " << setw(12) << "Cédula" << "| " << setw(35) << "Nombre Completo" << "| " << setw(22) << "Cuenta" << "|" << endl;
        cout << "-------------------------------------------------------------------------------\n";
        for (size_t i = 0; i < listaClientes.size(); ++i) {
            Cliente* c = listaClientes[i];
            cout << left<< "| " << setw(4)  << (i + 1)<< "| " << setw(12) << c->getCedula()<< "| " << setw(35) << (c->getPrimerNombre() + " " + c->getSegundoNombre() + " " + c->getPrimerApellido() + " " + c->getSegundoApellido()) << "| " << setw(22) << c->getCuenta()->getNumero()<< "|" << endl;
        }
        cout << "-------------------------------------------------------------------------------\n";
        string numero;
        int seleccion = 0;
        do {
            ValidacionDatos::limpiar_linea("• Ingrese el número del cliente (o ESC para volver): ");
            numero = ValidacionDatos::ingresar_dni("");
            if (numero == "__ESC__") return; // Volver
            if (!numero.empty()) {
                try {
                    seleccion = stoi(numero);
                } catch (...) {
                    seleccion = 0;
                }
            }
        } while (seleccion < 1 || seleccion > static_cast<int>(listaClientes.size()));
        cout << endl;
        // Validar selección
        Cliente* cliente = listaClientes[seleccion - 1];
        string nombres = cliente->getPrimerNombre() + " " + cliente->getSegundoNombre();
        string apellidos = cliente->getPrimerApellido() + " " + cliente->getSegundoApellido();
        string cuenta = cliente->getCuenta()->getNumero();
        string cedula = cliente->getCedula();
        // Carpeta de salida
        string carpeta = "QRCodes";
        CreateDirectoryA(carpeta.c_str(), NULL);

        string archivoPng = carpeta + "\\QR_" + cedula + ".png";
        string archivoPdf = carpeta + "\\QR_" + cedula + ".pdf";

        bool exito = generarQrPersonal(nombres, apellidos, cuenta, archivoPng, archivoPdf, 8);
        getch();
        while (true) {
            system("cls");
            const char* opciones[] = {"Sí, generar PDF", "Volver a la lista de clientes", "Atrás"};
            cout<<"\n\n\t\t=====================================\n";
            cout << "\t\t====== DESEA GENERAR PDF DEL QR =====\n";
            cout << "\t\t=====================================\n";
            cout << "\t\t SELECCIONE UNA OPCIÓN:\n";
            pausarMarquesina(); // Pausar marquesina antes de mostrar opciones
            int opcion = seleccionar_opcion("", opciones, 3, 4);
            reanudarMarquesina(); // Reanudar marquesina después de la selección
            if (opcion == 1) {
                PDFCreator pdf;
                pdf.crearPDFPersonal(cliente, carpeta,archivoPng);
            } else if (opcion == 2) {
                break; // Volver a la lista
            } else if (opcion == 3 || opcion == 27) {
                return; // Volver al menú admin
            }
        }
    }
}

/**
 * @brief Muestra el menú de administrador y permite gestionar el sistema.
 * @details Permite realizar backup, restaurar, gestionar clientes, cifrado, descifrado, verificar integridad, generar QR y PDF.
 */
void Banco::mostrarMenuAdmin() {
    const char* opciones[] = {"Backup", "Restaurar Backup", "Gestionar", "Cifrado", "Descifrado", "Base Datos", "Verificar Integridad", "Generar QR", "Generar PDF", "Atrás"};
    const char* camposBusqueda[] = {"Cédula", "Primer Nombre", "Segundo Nombre", "Primer Apellido", "Segundo Apellido", "Saldo", "Atrás"};
    const char* camposOrdenamiento[] = {"Cédula", "Primer Nombre", "Segundo Nombre", "Primer Apellido", "Segundo Apellido", "Saldo"};
    bool salir = false;
    while (!salir) {
        system("cls");
        pausarMarquesina(); // Pausar marquesina antes de mostrar el menú
        int seleccion = seleccionar_opcion("\t\t=== PANEL ADMINISTRADOR ===\n\n", opciones, 10, 2);
        reanudarMarquesina(); // Reanudar marquesina después de mostrar el menú
        if (seleccion == 10) { // Atrás
            salir = true;
            continue;
        }
        system("cls");
        try {
            if (seleccion == 3) { // Gestionar
                bool campoElegido = false;
                while (!campoElegido) {
                    system("cls");
                    pausarMarquesina(); // Pausar marquesina antes de mostrar el menú
                    int campoBusqueda = seleccionar_opcion("\t\t=== SELECCIONE CAMPO DE BÚSQUEDA ===\n\n", camposBusqueda, 7, 2);
                    reanudarMarquesina(); // Reanudar marquesina después de mostrar el menú
                    if (campoBusqueda == 7) {
                        campoElegido = true; // Atrás
                        continue;
                    }
                    system("cls");
                    string campoOriginal = camposBusqueda[campoBusqueda - 1];
                    string camposMayusculas = campoOriginal;
                    for (char& c : camposMayusculas) c = toupper((unsigned char)c);
                    imprimirCuadroBusqueda(camposMayusculas);
                    string entrada;
                    vector<Cliente*> resultados;
                    if (campoBusqueda - 1 == SALDO) {
                        entrada = ValidacionDatos::capturarEntrada(ValidacionDatos::MONTO, "Ingrese el saldo a buscar: $", 20);
                        if (entrada.empty()) continue; // Atrás
                        buscarPorRangoSaldo(stod(entrada), resultados);
                    } else {
                        string mensaje = "Ingrese " + string(camposBusqueda[campoBusqueda - 1]) + ": ";
                        int longitud = 50;
                        ValidacionDatos::TipoDato tipo = (campoBusqueda - 1 == CEDULA) ? ValidacionDatos::CEDULA_PREFIJO : ValidacionDatos::NOMBRE;
                        entrada = ValidacionDatos::capturarEntrada(tipo, mensaje, longitud);
                        if (entrada.empty()) continue; // Atrás
                        buscarPorPrefijo(static_cast<CampoBusqueda>(campoBusqueda - 1), entrada, resultados);
                    }
                    // Mostrar resultados y menú horizontal
                    bool mostrarResultados = true;
                    int campoOrdenamiento = 0;
                    while (mostrarResultados) {
                        system("cls");
                        cout << "\n\t___________________________________\n";
                        cout << "\t||     RESULTADOS DE LA BUSQUEDA   ||\n";
                        cout << "\t||_________________________________||\n";
                        // Mostrar menú horizontal
                        cout << "Ordenar por: ";
                        for (int i = 0; i < 6; ++i) {
                            cout << (i == campoOrdenamiento ? "[" : " ") << camposOrdenamiento[i] << (i == campoOrdenamiento ? "]" : " ");
                            if (i < 5) cout << " | ";
                        }
                        cout << endl << endl;
                        // Mostrar tabla ordenada
                        if (resultados.empty()) {
                            cout << "• No se encontraron coincidencias." << endl;
                        } else {
                            ordenarCuentas(resultados, static_cast<CampoBusqueda>(campoOrdenamiento));
                        }
                        cout << "\n• Use flechas izquierda/derecha para ordenar, Enter o Esc para volver.";
                        int tecla = _getch();
                        if (tecla == 224 || tecla == 0) {
                            tecla = _getch();
                            if (tecla == 75) campoOrdenamiento = (campoOrdenamiento - 1 + 6) % 6; // Izquierda
                            else if (tecla == 77) campoOrdenamiento = (campoOrdenamiento + 1) % 6; // Derecha
                        } else if (tecla == 13 || tecla == 27) { // Enter o Esc
                            mostrarResultados = false;
                        }
                    }
                }
            }  else if (seleccion == 1) { // Backup
                cout << "\n\t\t========================\n";
                cout << "\t\t=== REALIZAR BACKUP ====\n";
                cout << "\t\t=========================\n";
                cout << "Presione Esc para volver al menú de administrador.\n";
                if (clientes.estaVacia()) {
                    cout << "\t======================================================\n";
                    cout << "\t  ERROR AL REALIZAR BACKUP: No hay clientes registrados.\n";
                    cout << "\t======================================================\n";   
                } else if(BackupManager::validarArchivoExistente(clientes)) {
                    cout << "\t======================================================\n";
                    cout << "\t  ERROR AL REALIZAR BACKUP: Ya existe un backup igual.\n";
                    cout << "\t======================================================\n";
                } else {
                    string nombreArchivo = BackupManager::guardarBackup(clientes);
                    cout << "\t======================================================\n";
                    cout << "\t  BACKUP REALIZADO CON ÉXITO: "<<nombreArchivo << endl;
                    cout << "\t======================================================\n";
                }
            } else if (seleccion == 2) { // Restaurar Backup
                string fecha, fecha_hora;
                int horas=999, minutos=999, segundos=999,dia=0, mes=0, anio=0;
                try {
                cout << "\n\t\t========================\n";
                cout << "\t\t=== RESTAURAR BACKUP ===\n";
                cout << "\t\t========================\n";
                cout << "• Presione Esc para volver al menú de administrador.\n";
                do {
                    ValidacionDatos::limpiar_linea("• Ingrese la fecha del backup (DD/MM/YYYY): ");
                    fecha = ValidacionDatos::validar_Fecha("");
                    if (fecha == "__ESC__") break; // Atrás 
                    if (fecha.length() == 8) {
                        dia = stoi(fecha.substr(0, 2));
                        mes = stoi(fecha.substr(2, 2));
                        anio = stoi(fecha.substr(4, 4));
                    }
                } while (!ValidacionDatos::es_fecha_valida(anio, mes, dia));
                cout << endl;
                do {
                    ValidacionDatos::limpiar_linea("• Ingrese la hora del backup (HH:MM:SS): ");
                    fecha_hora = ValidacionDatos::validarHora("");
                    if (fecha_hora == "__ESC__") break; // Atrás
                    if (fecha_hora.length() == 6){
                        horas = stoi(fecha_hora.substr(0, 2));
                        minutos = stoi(fecha_hora.substr(2, 2));
                        segundos = stoi(fecha_hora.substr(4, 2));
                    }
                } while (!ValidacionDatos::validar_hora_minuto_segundo(horas, minutos, segundos));
                cout << endl;
                    char buffer[100];
                    snprintf(buffer, sizeof(buffer), "backup_%02d%02d%04d_%02d%02d%02d.txt", dia, mes, anio, horas, minutos, segundos);
                    string nombreArchivo = buffer;
                    //string nombreArchivo = ValidacionDatos::capturarEntrada(ValidacionDatos::NOMBRE_ARCHIVO, "Nombre del archivo de backup: ", 50);
                    if (nombreArchivo.empty()) continue; // Atrás
                    BackupManager::restaurarBackup(nombreArchivo, clientes);
                    cout << "\t\t===================================================\n";
                    cout << "\t\t=== BACKUP RESTAURADO: " << nombreArchivo << " ===\n";
                    cout << "\t\t===================================================\n";
                    cout << "Backup restaurado con éxito.\n";
                } catch (const BancoException& e) 
                {
                    cout << "\t===================================================\n";
                    cout << "\t  ERROR AL RECUPERAR BACKUP: " << e.what() << endl;
                    cout << "\t===================================================\n";
                    cout << "Regresando al menú principal...\n";
                }
            } else if (seleccion == 4) { // Cifrado
                CifradoCesar::cifrar_archivos_txt();
            } else if (seleccion == 5) { // Descifrado
                CifradoCesar::descifrar_archivos_txt();
            } else if (seleccion == 6) { // Base Datos
                mostrarBaseDatos();
            } else if (seleccion == 7) { // Verificar Integridad
                cout << "\n\t\t============================\n";
                cout << "\t\t=== VERIFICAR INTEGRIDAD ===\n";
                cout << "\t\t============================\n";
                cout << "Presione Esc para volver al menú de administrador.\n";
                CifradoCesar::verificarIntegridadCifrados();
            } else if (seleccion == 8) { // Generar QR
                generarQR();
            } else if (seleccion == 9) { // Generar PDF
                CifradoCesar::generar_txt_a_pdf();
            }
        } catch (const BancoException& e) {
            cout << "\nError: " << e.what() << endl;
        }
        cout << "\nPresiona Enter para continuar...";
        getch();
    }
}

/**
 * @brief Muestra la base de datos de clientes y cuentas en formato tabular.
 */
void Banco::mostrarBaseDatos() {
    system("cls");
    if (clientes.estaVacia()) {
        cout << "\n\t\t===================================\n";
        cout << "\t\t=== NO HAY CLIENTES REGISTRADOS ===\n";
        cout << "\t\t===================================\n";
        return;
    }
    // Encabezado
    cout << "\n\n\n\t\t===========================================\n";
    cout << "\t\t===    LISTADO DE CLIENTES Y CUENTAS    ===\n";
    cout << "\t\t===========================================\n";
    cout << "Presione Esc para volver al menú de administrador.\n";
    cout << "---------------------------------------------------------------------------------------------------------------\n";
    cout << left << "| " << setw(4)  << "#"  << "| " << setw(12) << "Cédula" << "| " << setw(16) << "Nombre" << "| " << setw(18) << "Apellido" << "| " << setw(12) << "Teléfono" << "| " << setw(25) << "Correo" << "| " << setw(22) << "Cuenta" << "| " << setw(10) << "Saldo" << "|" << endl;
    cout << "---------------------------------------------------------------------------------------------------------------\n";

    int i = 1;
    clientes.recorrer([&](Cliente* cliente) {
        cout << left << "| " << setw(4)  << i++ << "| " << setw(12) << cliente->getCedula()<< "| " << setw(16) << (cliente->getPrimerNombre() + " " + cliente->getSegundoNombre())
             << "| " << setw(18) << (cliente->getPrimerApellido() + " " + cliente->getSegundoApellido())
             << "| " << setw(12) << cliente->getTelefono()
             << "| " << setw(25) << cliente->getCorreo()
             << "| " << setw(22) << cliente->getCuenta()->getNumero()
             << "| " << setw(10) << fixed << setprecision(2) << cliente->getCuenta()->getSaldo()
             << "|" << endl;
    });

    cout << "---------------------------------------------------------------------------------------------------------------\n";
    cout << "\nTotal de clientes: " << i - 1 << endl;
    cout << "===========================================\n";
}

/**
 * @brief Genera un número de cuenta único y válido para un tipo de cuenta.
 * @param tipoCuenta "Ahorros" o "Corriente".
 * @return Número de cuenta generado (formato IBAN).
 */
string Banco::generarNumeroCuenta(const string& tipoCuenta) {
    static long long contador = 10000000; // Iniciar en 10000000
    string prefijo = (tipoCuenta == "Ahorros") ? "22" : "23";

    while (true) {
        ostringstream oss;
        oss << setfill('0') << setw(8) << contador;
        string numCuenta = prefijo + oss.str();
        string ibanParcial = "EC0030100001" + numCuenta;
        string codigoControl = calcularCodigoControl(ibanParcial);
        string iban = "EC" + codigoControl + "30100001" + numCuenta;

        // Verificar unicidad
        bool existe = false;
        clientes.recorrer([&](Cliente* c) {
            if (c->getCuenta()->getNumero() == iban) existe = true;
        });
        if (!existe) {
            contador++;
            return iban;
        }
        contador++;
    }
}

/**
 * @brief Ejecuta el menú principal del sistema bancario.
 */
void Banco::ejecutar() {
    const int NUM_OPCIONES = 7;
    int contador = 6; 
    const char* OPCIONES[NUM_OPCIONES] = {
        "Ingresar como Admin",
        "Ingresar como Usuario",
        "Crear Cuenta",
        "Recuperar Cuenta",
        "Más Información",
        "Documentación",
        "Salir"
    };
    marquesina.iniciar(); // Iniciar marquesina

    try {
    int opcion;
    do {
        system("cls");
        cout << "\n\t\t  _________________________________\n";
        cout << "\t\t  ||        BANCO CONFIANZA      ||\n";
        cout << "\t\t  ||    Bienvenido al sistema    ||\n";
        cout << "\t\t  ||_____________________________||\n";
        cout << "\n\t\t=== MENÚ PRINCIPAL ===\n";
        pausarMarquesina();
        opcion = seleccionar_opcion("", OPCIONES, NUM_OPCIONES, contador);
		reanudarMarquesina();
        switch (opcion) {
                case 1: // Administrador
                    mostrarMenuAdmin();
                    break;
                case 2: // Iniciar sesión (Usuario)
                    mostrarMenuUsuario();
                    break;
                case 3:// Crear cuenta
                    crearCuenta(); 
                    break;
                case 4: // Recuperar Cuenta
                    recuperarCuenta(clientes);
                    break;
                case 5: // Más Información
                    mostrar_ayuda_tecnica();
                    break;
                case 6: // Documentación
                    generarDocumentacion();
                    break;
                case 7: // Salir
                    return;
            }
            reanudarMarquesina();
        } while (opcion != NUM_OPCIONES); // 7 = Salir
    } catch (const BancoException& e) {
        cout << "\nError: " << e.what() << endl;
        cout << "\nPresiona Enter para volver...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }
}

