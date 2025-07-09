/**
 * @brief Clase Hash para gestionar valores originales, 
 * hash (MD5) y valores numéricos, con métodos para obtener y 
 * modificar datos. Incluye TablaHash, una estructura basada en 
 * vector de listas doblemente enlazadas circulares para manejar 
 * colisiones, con funciones de inserción, búsqueda y eliminación 
 * usando un hash simple.
 */
#ifndef HASH_H
#define HASH_H
#include "ListaDobleCircular.h"
#include "Cliente.h"
#include "Banco.h"
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

using namespace std;

class Hash {
    string valor_original;
    string valor_hashed;
    int value;
public:
    Hash(const string& valor, int val) : valor_original(valor), value(val) {
        valor_hashed = calcularMD5(valor); // o la función hash que uses
    }
    string getOriginal() const { return valor_original; }
    string getHash() const { return valor_hashed; }
    int getValue() const { return value; }
    void setValue(int val) { value = val; }

    static string calcularMD5(const string& str) {
    // --- INICIO IMPLEMENTACIÓN MD5 ---
    typedef unsigned int uint32;
    static const uint32 s[] = { 7,12,17,22, 5,9,14,20, 4,11,16,23, 6,10,15,21 };
    static const uint32 K[] = {
        0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,
        0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,0x6b901122,0xfd987193,0xa679438e,0x49b40821,
        0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
        0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,
        0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
        0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,
        0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,
        0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391
    };
    auto leftrotate = [](uint32 x, uint32 c) { return (x << c) | (x >> (32 - c)); };

    // Inicializar variables
    uint32 a0 = 0x67452301, b0 = 0xefcdab89, c0 = 0x98badcfe, d0 = 0x10325476;

    // Preprocesamiento
    vector<unsigned char> msg(str.begin(), str.end());
    size_t orig_len_bits = msg.size() * 8;
    msg.push_back(0x80);
    while ((msg.size() * 8) % 512 != 448) msg.push_back(0);
    for (int i = 0; i < 8; ++i) msg.push_back((orig_len_bits >> (8 * i)) & 0xFF);

    // Procesar en bloques de 512 bits
    for (size_t offset = 0; offset < msg.size(); offset += 64) {
        uint32 M[16];
        for (int i = 0; i < 16; ++i) {
            M[i] = 0;
            for (int j = 0; j < 4; ++j)
                M[i] |= msg[offset + i * 4 + j] << (j * 8);
        }
        uint32 A = a0, B = b0, C = c0, D = d0;
        for (int i = 0; i < 64; ++i) {
            uint32 F, g;
            if (i < 16) { F = (B & C) | (~B & D); g = i; }
            else if (i < 32) { F = (D & B) | (~D & C); g = (5 * i + 1) % 16; }
            else if (i < 48) { F = B ^ C ^ D; g = (3 * i + 5) % 16; }
            else { F = C ^ (B | ~D); g = (7 * i) % 16; }
            uint32 temp = D;
            D = C;
            C = B;
            B = B + leftrotate(A + F + K[i] + M[g], s[(i / 16) * 4 + (i % 4)]);
            A = temp;
        }
        a0 += A; b0 += B; c0 += C; d0 += D;
    }
    stringstream ss;
    for (uint32 v : {a0, b0, c0, d0})
        for (int i = 0; i < 4; ++i)
            ss << hex << setw(2) << setfill('0') << ((v >> (8 * i)) & 0xFF);
    return ss.str();
    // --- FIN IMPLEMENTACIÓN MD5 ---
    }
    /*
    static string calcularSHA256(const string& str) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, str.c_str(), str.size());
        SHA256_Final(hash, &sha256);
        // Convertir el hash a una cadena hexadecimal
        stringstream ss;
        for(int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
        return ss.str();
    }
    */
};

// Tabla hash
class TablaHash {
    // Usamos ListaDoble para manejar colisiones
    vector<ListaDobleCircular<Hash>*> table;
    // Capacidad de la tabla hash
    int capacidad;
    int hashFunction(const string& key) {
        int hash = 0;
        for (char c : key) hash = (hash * 31 + c) % capacidad;
        return hash;
    }
    public:
    TablaHash(int cap) : capacidad(cap), table(cap, new ListaDobleCircular<Hash>()) {}
    ~TablaHash() {
        for (auto* lista : table) {
            delete lista;
        }
    }
    // Inserta un nuevo elemento o actualiza el valor si ya existe
    void insert(const string& key, int value) {
        int idx = hashFunction(key);
        auto* lista = table[idx];
        auto* nodo = lista->buscar([&](Hash* e){ return e->getOriginal() == key; });
        if (nodo) {
            nodo->setValue(value);
        } else {
            lista->insertar(new Hash(key, value));
        }
    }
    bool get(const string& key, int& value) {
        int idx = hashFunction(key);
        auto* lista = table[idx];
        auto* nodo = lista->buscar([&](Hash* e){ return e->getOriginal() == key; });
        if (nodo) {
            value = nodo->getValue();
            return true;
        }
        return false;
    }
    void remove(const string& key) {
        int idx = hashFunction(key);
        auto* lista = table[idx];
        auto* nodo = lista->buscar([&](Hash* e){ return e->getOriginal() == key; });
        if (nodo) {
            lista->eliminar(nodo);
        }
    }
};
#endif // HASH_H