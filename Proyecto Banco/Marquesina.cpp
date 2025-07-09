#include "Marquesina.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <regex>
#include <string>

/**
 * @brief Constructor de la clase Marquesina
 * @param x Posición X en la consola
 * @param y Posición Y en la consola
 * @param ancho Ancho en caracteres de la marquesina
 * @param archivoHTML Ruta del archivo HTML que contiene el contenido de la marquesina
 * @param velocidad Velocidad de desplazamiento del texto (en milisegundos)
 */
Marquesina::Marquesina(int x, int y, int ancho, const std::string& archivoHTML, int velocidad)
	: posX(x), posY(y), ancho(ancho), archivoHTML(archivoHTML), velocidad(velocidad),
	ejecutando(false), pausado(false), bloqueado(false), operacionCritica(false),
	bufferActualizado(false), bufferListo(false)
{
	// Inicializar buffers con espacio suficiente
	bufferPrimario.resize(ancho, { ' ', FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY });
	bufferSecundario.resize(ancho, { ' ', FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY });

	ZeroMemory(&ultimaModificacion, sizeof(FILETIME));
	cargarDesdeHTML();
	actualizarBuffer();
}

/**
 * @brief Destructor de la clase Marquesina
 *
 * Detiene el hilo de la marquesina si está en ejecución
 */
Marquesina::~Marquesina()
{
	detener();
}

/**
 * @brief Inicia la animación de la marquesina en un hilo separado
 */
void Marquesina::iniciar()
{
	if (!ejecutando)
	{
		ejecutando = true;
		pausado = false;
		bloqueado = false;
		operacionCritica = false;
		hiloMarquesina = std::thread(&Marquesina::ejecutarMarquesina, this);
	}
}

/**
 * @brief Detiene la animación de la marquesina y finaliza el hilo
 */
void Marquesina::detener()
{
	if (ejecutando)
	{
		ejecutando = false;
		if (hiloMarquesina.joinable())
		{
			hiloMarquesina.join();
		}
	}
}

/**
 * @brief Pausa temporalmente la animación de la marquesina
 */
void Marquesina::pausar()
{
	pausado = true;
}

/**
 * @brief Reanuda la animación de la marquesina después de una pausa
 */
void Marquesina::reanudar()
{
	pausado = false;
}

/**
 * @brief Bloquea la marquesina impidiendo cualquier actualización
 */
void Marquesina::bloquear()
{
	bloqueado = true;
}

/**
 * @brief Desbloquea la marquesina permitiendo actualizaciones
 */
void Marquesina::desbloquear()
{
	bloqueado = false;
}

/**
 * @brief Marca el inicio de una operación crítica que no debe ser interrumpida
 */
void Marquesina::marcarOperacionCritica()
{
	operacionCritica = true;
}

/**
 * @brief Marca el fin de una operación crítica
 */
void Marquesina::finalizarOperacionCritica()
{
	operacionCritica = false;
}

/**
 * @brief Actualiza el contenido del buffer de la marquesina
 */
void Marquesina::forzarActualizacion()
{
	std::lock_guard<std::mutex> lock(mtx);
	cargarDesdeHTML();
	actualizarBuffer();
}

/**
 * @brief Función principal que se ejecuta en el hilo de la marquesina
 */
void Marquesina::actualizarTexto(const std::string& nuevoTexto)
{
	std::lock_guard<std::mutex> lock(mtx);
	std::ofstream archivo(archivoHTML);
	if (archivo)
	{
		archivo << "<marquesina>\n";
		archivo << "  " << nuevoTexto << "\n";
		archivo << "</marquesina>";
		archivo.close();

		// Recargar el contenido
		cargarDesdeHTML();
	}
}


std::wstring stringToWideString(const std::string& str)
{
	if (str.empty())
		return L"";

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size_needed);
	return wstr;
}

void Marquesina::mostrarRutaArchivo()
{
	char path[MAX_PATH];
	GetFullPathNameA(archivoHTML.c_str(), MAX_PATH, path, NULL);
	std::cout << "Ruta completa del archivo HTML: " << path << std::endl;
}

// Modificar la funcion cargarDesdeHTML
void Marquesina::cargarDesdeHTML()
{
	std::ifstream archivo(archivoHTML);
	if (!archivo)
	{
		std::ofstream crear(archivoHTML);
		if (crear)
		{
			crear << "<marquesina>\n";
			crear << "  <color=white bg=blue>Bienvenido al Sistema Bancario</color> - ";
			crear << "<color=yellow>Desarrolladores: Uriel Andrade, Kerly Chuqui, Abner Proano</color>\n";
			crear << "</marquesina>";
			crear.close();
			archivo.open(archivoHTML);
		}
	}

	if (archivo)
	{
		std::string contenido;
		std::string linea;
		while (std::getline(archivo, linea))
		{
			contenido += linea + "\n";
		}
		elementos = parsearHTML(contenido);

		std::wstring wArchivoHTML = stringToWideString(archivoHTML);
		HANDLE hFile = CreateFileW(
			wArchivoHTML.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			GetFileTime(hFile, NULL, NULL, &ultimaModificacion);
			CloseHandle(hFile);
		}
	}
}

bool Marquesina::archivoModificado()
{
	FILETIME nuevoTiempo;
	ZeroMemory(&nuevoTiempo, sizeof(FILETIME));

	std::wstring wArchivoHTML = stringToWideString(archivoHTML);
	HANDLE hFile = CreateFileW(
		wArchivoHTML.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		GetFileTime(hFile, NULL, NULL, &nuevoTiempo);
		CloseHandle(hFile);

		if (CompareFileTime(&nuevoTiempo, &ultimaModificacion) > 0)
		{
			ultimaModificacion = nuevoTiempo;
			return true;
		}
	}

	return false;
}

std::vector<ElementoMarquesina> Marquesina::parsearHTML(const std::string& contenido)
{
	std::vector<ElementoMarquesina> resultado;

	std::regex marquesinaRegex("<marquesina>[\\s\\S]*?</marquesina>");
	std::regex colorRegex("<color=([a-z]+)(?:\\s+bg=([a-z]+))?>(.*?)</color>");
	std::regex boldRegex("<b>(.*?)</b>");

	std::smatch matchMarquesina;
	if (std::regex_search(contenido, matchMarquesina, marquesinaRegex))
	{
		std::string textoMarquesina = matchMarquesina.str();
		textoMarquesina = std::regex_replace(textoMarquesina, std::regex("<marquesina>"), "");
		textoMarquesina = std::regex_replace(textoMarquesina, std::regex("</marquesina>"), "");

		std::string::const_iterator searchStart(textoMarquesina.cbegin());
		std::string::const_iterator end(textoMarquesina.cend());
		std::smatch match;

		while (std::regex_search(searchStart, end, match, colorRegex))
		{
			ElementoMarquesina elem;

			std::string colorTexto = match[1].str();
			if (colorTexto == "black") elem.colorTexto = 0;
			else if (colorTexto == "blue") elem.colorTexto = 1;
			else if (colorTexto == "green") elem.colorTexto = 2;
			else if (colorTexto == "cyan") elem.colorTexto = 3;
			else if (colorTexto == "red") elem.colorTexto = 4;
			else if (colorTexto == "magenta") elem.colorTexto = 5;
			else if (colorTexto == "yellow") elem.colorTexto = 6;
			else if (colorTexto == "white") elem.colorTexto = 7;
			else elem.colorTexto = 7;

			elem.colorFondo = 0;
			if (match[2].matched)
			{
				std::string colorFondo = match[2].str();
				if (colorFondo == "black") elem.colorFondo = 0;
				else if (colorFondo == "blue") elem.colorFondo = 1;
				else if (colorFondo == "green") elem.colorFondo = 2;
				else if (colorFondo == "cyan") elem.colorFondo = 3;
				else if (colorFondo == "red") elem.colorFondo = 4;
				else if (colorFondo == "magenta") elem.colorFondo = 5;
				else if (colorFondo == "yellow") elem.colorFondo = 6;
				else if (colorFondo == "white") elem.colorFondo = 7;
			}

			std::string contenidoColor = match[3].str();
			std::smatch boldMatch;
			if (std::regex_search(contenidoColor, boldMatch, boldRegex))
			{
				elem.negrita = true;
				elem.texto = boldMatch[1].str();
			}
			else
			{
				elem.negrita = false;
				elem.texto = contenidoColor;
			}

			resultado.push_back(elem);
			searchStart = match.suffix().first;
		}

		if (resultado.empty())
		{
			ElementoMarquesina elem;
			elem.colorTexto = 7;
			elem.colorFondo = 0;
			elem.negrita = false;
			elem.texto = textoMarquesina;
			resultado.push_back(elem);
		}
	}

	return resultado;
}

void Marquesina::renderizarMarquesina()
{
	// Fase 1: Preparar el contenido en el buffer primario
	std::string textoBuffer;
	{
		std::lock_guard<std::mutex> lock(mtx);
		textoBuffer = bufferMarquesina;
	}

	if (textoBuffer.empty())
		return;

	// Calcular posición actual del texto
	static int posicionTexto = 0;
	std::string textoVisible = textoBuffer.substr(posicionTexto, ancho);
	if (textoVisible.length() < ancho) {
		textoVisible += textoBuffer.substr(0, ancho - textoVisible.length());
	}

	// Preparar el buffer primario
	{
		std::lock_guard<std::mutex> lock(bufferMutex);
		WORD colorTurquesa = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;

		for (size_t i = 0; i < ancho; i++) {
			if (i < textoVisible.length()) {
				bufferPrimario[i] = { textoVisible[i], colorTurquesa };
			}
			else {
				bufferPrimario[i] = { ' ', colorTurquesa };
			}
		}

		// Avanzar la posición para la próxima actualización
		posicionTexto = (static_cast<unsigned long long>(posicionTexto) + 1) % textoBuffer.length();

		// Intercambiar los buffers
		bufferPrimario.swap(bufferSecundario);
		bufferListo = true;
	}

	// Fase 2: Transferir el buffer a la consola en una única operación atómica
	if (bufferListo.load()) {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbi;

		if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
			return;

		// Crear arrays temporales para la API de Windows
		std::vector<CHAR_INFO> charInfoArray(ancho);
		{
			std::lock_guard<std::mutex> lock(bufferMutex);
			for (size_t i = 0; i < ancho; i++) {
				charInfoArray[i].Char.AsciiChar = bufferSecundario[i].first;
				charInfoArray[i].Attributes = bufferSecundario[i].second;
			}
		}

		// Definir región de origen y destino
		COORD bufferSize = { static_cast<SHORT>(ancho), 1 };
		COORD bufferCoord = { 0, 0 };
		SMALL_RECT writeRegion = {
			static_cast<SHORT>(posX),
			static_cast<SHORT>(posY),
			static_cast<SHORT>(posX + ancho - 1),
			static_cast<SHORT>(posY)
		};

		// Escribir todo el buffer de una vez usando WriteConsoleOutput
		WriteConsoleOutputA(
			hConsole,
			charInfoArray.data(),
			bufferSize,
			bufferCoord,
			&writeRegion
		);

		bufferListo = false;
	}
}

void Marquesina::actualizarBuffer()
{
	std::lock_guard<std::mutex> lock(mtx);
	bufferMarquesina = "   ";
	for (const auto& elem : elementos)
	{
		bufferMarquesina += elem.texto + "   ";
	}
	bufferActualizado = true;
}

void Marquesina::ejecutarMarquesina()
{
	while (ejecutando)
	{
		// Verificaciones múltiples antes de renderizar
		if (bloqueado || pausado || elementos.empty()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		// Verificar actualización de archivo
		if (archivoModificado()) {
			cargarDesdeHTML();
			actualizarBuffer();
		}

		// Renderizar con el buffer doble para evitar conflictos
		try {
			renderizarMarquesina();
		}
		catch (...) {
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			continue;
		}

		// Velocidad controlada
		std::this_thread::sleep_for(std::chrono::milliseconds(velocidad));
	}
}

int Marquesina::get_cursor_row() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.dwCursorPosition.Y;
}
