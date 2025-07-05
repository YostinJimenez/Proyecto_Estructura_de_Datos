#define WIN32_LEAN_AND_MEAN
#include "Banco.h"
#include "Marquesina.h"
#include <iostream>
#include <string>
#include <limits>
#include <conio.h>
#include <windows.h>
#include <shellapi.h>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include "ValidacionDatos.h"
#include "CuentaAhorros.h"
#include "CuentaCorriente.h"
#include "recuperarCuenta.h"
#include "ListaDobleCircular.h"
#include "BackupManager.h"
using namespace std;
extern Marquesina marquesina;
// Función auxiliar para calcular el código de control del IBAN
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
// Constructor de Banco
Banco::Banco() {
    // Inicialización por defecto de clientes (ListaDobleCircular ya se inicializa sola)
}

void Banco::mostrar_ayuda_tecnica()
{
    marquesina.pausar();
    system("cls");
    try
    {
        cout << "\n\n\t\t===========================================" << endl;
        cout << "\t\t===    MANUAL DE AYUDA TÉCNICA BANCO    ===" << endl;
        cout << "\t\t===========================================" << endl;
        HINSTANCE result = ShellExecute(NULL, "open", "AyudaTecnicaBanco.chm", NULL, NULL, SW_SHOWNORMAL);
        if (result <= (HINSTANCE)32)
        {
            throw runtime_error("No se pudo abrir el archivo de ayuda. Asegúrese de que 'AyudaTecnicaBanco.chm' esté en el directorio del programa.");
        }
        cout << "Manual de Ayuda Técnica abierto exitosamente.\n";
        marquesina.reanudar(); // Reanudar marquesina después de abrir ayuda
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

void Banco::mover_cursor(int x, int y){
    lock_guard<mutex> lock(consola_mutex);
    HANDLE manejo_consola = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {static_cast<SHORT>(x), static_cast<SHORT>(y)};
    SetConsoleCursorPosition(manejo_consola, pos);
}

void Banco::mover_cursor_opciones(int x, int y){
    marquesina.iniciar(); // Asegurarse de que la marquesina esté activa
    marquesina.pausar(); // Pausar marquesina antes de mover el cursor
    lock_guard<mutex> lock(consola_mutex);
    HANDLE manejo_consola = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {static_cast<SHORT>(x), static_cast<SHORT>(y)};
    SetConsoleCursorPosition(manejo_consola, pos);
    marquesina.reanudar(); // Reanudar marquesina después de mover el cursor
}

int Banco::seleccionar_opcion(const char *titulo, const char *opciones[], int n, int fila_inicio) {
    system("chcp 65001 > nul");
    int opcionSeleccionada = 1;
    int tecla;

    // Dibujar el título, alineado a la izquierda, en la fila especificada
    mover_cursor_opciones(1, fila_inicio);
    cout << titulo;

    // Dibujar las opciones iniciales, empezando justo debajo del título
    for (int e = 0; e < n; e++) {
        mover_cursor_opciones(10, fila_inicio + 2 + e); // +2 para dejar un espacio entre título y opciones
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
                mover_cursor_opciones(10, fila_inicio + 2 + (opcionSeleccionada - 1));
                cout << "    " << opcionSeleccionada << " " << opciones[opcionSeleccionada - 1];

                opcionSeleccionada--;
                if (opcionSeleccionada < 1) opcionSeleccionada = n;

                mover_cursor_opciones(10, fila_inicio + 2 + (opcionSeleccionada - 1));
                cout << " ➤ " << opcionSeleccionada << " " << opciones[opcionSeleccionada - 1];
                break;

            case TECLA_ABAJO:
                // Actualizar la opción seleccionada
                mover_cursor_opciones(10, fila_inicio + 2 + (opcionSeleccionada - 1));
                cout << "    " << opcionSeleccionada << " " << opciones[opcionSeleccionada - 1];

                opcionSeleccionada++;
                if (opcionSeleccionada > n) opcionSeleccionada = 1;

                mover_cursor_opciones(10, fila_inicio + 2 + (opcionSeleccionada - 1));
                cout << " ➤ " << opcionSeleccionada << " " << opciones[opcionSeleccionada - 1];
                break;
            }
    } while (tecla != ENTER);

    return opcionSeleccionada;
}

bool Banco::mostrarConfirmacion(double monto, const string& operacion) {
    system("cls");
    const char* opciones[] = {"Aceptar", "Cancelar"};
    cout <<"\t=====================================\n";
    cout << "\t======= CONFIRMAR " << operacion << " =======\n";
    cout <<"\t=====================================\n";

    cout << "\tMonto: $" << fixed << setprecision(2) << monto << "\n";
    cout << "\t¿Está seguro?\n\n";
    int seleccion = seleccionar_opcion("", opciones, 2, 3);
    if (seleccion == 2) return false; // Atrás
    return (seleccion == 1); // Aceptar (true), Cancelar (false)
}

string Banco::seleccionarTipoCuenta() {
    system("cls");
    const char* opciones[] = {"Ahorros", "Corriente", "Atrás"};
    int seleccion = seleccionar_opcion("\t\t=== SELECCIONE TIPO DE CUENTA ===\n\n", opciones, 3, 2);
    if (seleccion == 3) return ""; // Atrás
    return opciones[seleccion - 1];
}

void Banco::crearCuenta() {
    marquesina.pausar(); // Pausar marquesina antes de pedir datos
    system("cls");
    cout << "=== CREACION DE CUENTA ===\n\n";
    cout << "Presione Esc para volver al menú principal.\n\n";
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
        marquesina.pausar(); // Pausar marquesina antes de continuar
        system("cls");
        cout << "=== CREACION DE CUENTA ===\n\n";
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
    marquesina.reanudar(); 
}

void Banco::mostrarMenuUsuario() {
    marquesina.pausar(); // Pausar marquesina antes de pedir datos
    system("cls");
    try {
        cout<<"\t=====================================\n";
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
        if (!cliente) throw BancoException("Usuario o contrasena incorrectos.");
        mostrarMenuUsuario(cliente);
    } catch (const BancoException& e) {
        cout << "\nError: " << e.what() << "\n";
        cout << "\nPresiona Enter para volver...";
        getch();
    }
}

void Banco::mostrarMenuUsuario(Cliente* cliente) {
    const char* opciones[] = {"Transferir", "Depositar", "Retirar", "Consultar Transacciones", "Atrás"};
    bool salir = false;
    while (!salir) {
        system("cls");
        cout << "\t\t=== MENU USUARIO ===\n\n";
        Cuenta* cuenta = cliente->getCuenta();
        string tipoCuenta = dynamic_cast<CuentaAhorros*>(cuenta) ? "Ahorros" : "Corriente";
        cout << "\tNúmero de cuenta: " << cuenta->getNumero() << "\n";
        cout << "\tTipo de cuenta: " << tipoCuenta << "\n";
        cout << "\tSaldo: $" << fixed << setprecision(2) << cuenta->getSaldo() << "\n\n";
        int seleccion = seleccionar_opcion("", opciones, 5, 4);
        marquesina.pausar();
        if (seleccion == 5) {
            salir = true;
            continue; // Atrás
        }
        system("cls");
        try {
            switch (seleccion) {
                case 1: { // Transferir
                    cout << "=== REALIZAR TRANSFERENCIA ===\n\n";
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
                    cout << "=== REALIZAR DEPOSITO ===\n\n";
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
                    cout << "=== REALIZAR RETIRO ===\n\n";
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
                    seleccion = seleccionar_opcion("\t\t=== CONSULTAR TRANSACCIONES ===\n\n", opciones_Transacciones, 4, 2);
                    marquesina.pausar();
                    string tituloConsulta = "=== CONSULTAR TRANSACCIONES ===\n\nPresione Esc para volver al menú de usuario.\n\n";
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

void Banco::mostrarMenuAdmin() {
    const char* opciones[] = {"Backup", "Restaurar Backup", "Gestionar", "Cifrado", "Descifrado", "Base Datos", "Verificar Integridad","Atrás"};
    const char* camposBusqueda[] = {"Cédula", "Primer Nombre", "Segundo Nombre", "Primer Apellido", "Segundo Apellido", "Saldo", "Atrás"};
    const char* camposOrdenamiento[] = {"Cédula", "Primer Nombre", "Segundo Nombre", "Primer Apellido", "Segundo Apellido", "Saldo"};
    bool salir = false;
    while (!salir) {
        system("cls");
        int seleccion = seleccionar_opcion("\t\t=== PANEL ADMINISTRADOR ===\n\n", opciones, 8, 2);
        if (seleccion == 8) { // Atrás
            salir = true;
            continue;
        }
        marquesina.pausar(); // Pausar marquesina antes de mostrar el menú
        system("cls");
        try {
            if (seleccion == 3) { // Gestionar
                bool campoElegido = false;
                while (!campoElegido) {
                    system("cls");
                    int campoBusqueda = seleccionar_opcion("\t\t=== SELECCIONE CAMPO DE BÚSQUEDA ===\n\n", camposBusqueda, 7, 2);
                    if (campoBusqueda == 7) {
                        campoElegido = true; // Atrás
                        continue;
                    }
                    marquesina.pausar(); // Pausar marquesina antes de mostrar el menú
                    system("cls");
                    string campoOriginal = camposBusqueda[campoBusqueda - 1];
                    string camposMayusculas = campoOriginal;
                    for (char& c : camposMayusculas) c = toupper((unsigned char)c);
                    cout << "\t\t=== BÚSQUEDA POR " << camposMayusculas << " ===\n\n";
                    cout << "• Presione Esc para volver a la selección de campo.\n\n";
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
                        cout << "=== RESULTADOS DE BÚSQUEDA ===\n\n";
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
                cout << "\n\n\t\t========================\n";
                cout << "\t\t=== REALIZAR BACKUP ====\n";
                cout << "\t\t=========================\n";
                cout << "Presione Esc para volver al menú de administrador.\n";
                if (clientes.estaVacia()) {
                    cout << "No hay clientes registrados. No se puede realizar el backup.\n";
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
                marquesina.reanudar(); // Reanudar marquesina después de mostrar el menú
            } else if (seleccion == 2) { // Restaurar Backup
                string fecha, fecha_hora;
                int horas=999, minutos=999, segundos=999,dia=0, mes=0, anio=0;
                try {
                cout << "\t\t========================\n";
                cout << "\t\t=== RESTAURAR BACKUP ===\n";
                cout << "\t\t========================\n";
                cout << "• Presione Esc para volver al menú de administrador.\n";
                do {
                    ValidacionDatos::limpiar_linea("• Ingrese la fecha del backup (DD/MM/YYYY): ");
                    fecha = ValidacionDatos::validarFecha("");
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
                    cout << "\n\t\t===================================================\n";
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
                marquesina.reanudar();
            } else if (seleccion == 7) { // Verificar Integridad
                cout << "\n\n\t\t============================\n";
                cout << "\t\t=== VERIFICAR INTEGRIDAD ===\n";
                cout << "\t\t============================\n";
                cout << "Presione Esc para volver al menú de administrador.\n";
                CifradoCesar::verificarIntegridadCifrados();
                marquesina.reanudar();
            }   
        } catch (const BancoException& e) {
            cout << "\nError: " << e.what() << endl;
        }
        cout << "\nPresiona Enter para continuar...";
        getch();
    }
}

void Banco::mostrarBaseDatos() {
    system("cls");
    if (clientes.estaVacia()) {
        cout << "\n\n\t\t===================================\n";
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
    cout << left
         << "| " << setw(4)  << "#" 
         << "| " << setw(12) << "Cédula" 
         << "| " << setw(16) << "Nombre" 
         << "| " << setw(18) << "Apellido" 
         << "| " << setw(12) << "Teléfono" 
         << "| " << setw(25) << "Correo" 
         << "| " << setw(22) << "Cuenta" 
         << "| " << setw(10) << "Saldo"
         << "|" << endl;
    cout << "---------------------------------------------------------------------------------------------------------------\n";

    int i = 1;
    clientes.recorrer([&](Cliente* cliente) {
        cout << left
             << "| " << setw(4)  << i++
             << "| " << setw(12) << cliente->getCedula()
             << "| " << setw(16) << (cliente->getPrimerNombre() + " " + cliente->getSegundoNombre())
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

void Banco::ejecutar() {
    const int NUM_OPCIONES = 7;
    int contador = 6; 
    const char* OPCIONES[NUM_OPCIONES] = {
        "Ingresar como Admin",
        "Ingresar como Usuario",
        "Crear Cuenta",
        "Recuperar Cuenta",
        "Más Información",
        "Ayuda",
        "Salir"
    };

    try {
    int opcion;
    do {
        marquesina.pausar();
        system("cls");
        cout << "\n\n\t\t  _________________________________\n";
        cout << "\t\t  ||        BANCO CONFIANZA      ||\n";
        cout << "\t\t  ||    Bienvenido al sistema    ||\n";
        cout << "\t\t  ||_____________________________||\n";
        cout << "\n\t\t=== MENÚ PRINCIPAL ===\n\n";
        opcion = seleccionar_opcion("", OPCIONES, NUM_OPCIONES, contador);
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
                case 6: // Ayuda
                    system("cls");
                    cout << "\n\n\t\t========================\n";
                    cout << "\t\t=== AYUDA TÉCNICA ====\n";
                    cout << "\t\t========================\n";
                    cout << "Bienvenido al sistema del Banco Confianza.\n";
                    cout << "1) Ingresar como Admin: Acceso al panel de administrador para backups.\n";
                    cout << "2) Ingresar como Usuario: Inicie sesión con su usuario y contraseña.\n";
                    cout << "3) Crear Cuenta: Registre una nueva cuenta de ahorros o corriente.\n";
                    cout << "4) Recuperar Cuenta: Solicite la recuperación de su cuenta.\n";
                    cout << "5) Más Información: Consulte la documentación detallada en el navegador.\n";
                    cout << "6) Ayuda: Vea esta información.\n";
                    cout << "7) Salir: Cierre el sistema.\n";
                    cout << "\nUse las flechas arriba/abajo para navegar y Enter para seleccionar.\n";
                    cout << "\nPresiona Enter para volver...";
                    getch();
                    break;
                case 7: // Salir
                    return;
            }
        } while (opcion != NUM_OPCIONES); // 7 = Salir
    } catch (const BancoException& e) {
        cout << "\nError: " << e.what() << endl;
        cout << "\nPresiona Enter para volver...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }
}

