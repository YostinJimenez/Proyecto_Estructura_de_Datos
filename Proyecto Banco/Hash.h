#ifndef HASH_H
#define HASH_H
#include "ListaDobleCircular.h"
#include "Cliente.h"
#include "Banco.h"
#include <openssl/evp.h>
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
        unsigned char digest[EVP_MAX_MD_SIZE];
        unsigned int digest_len = 0;
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        EVP_DigestInit_ex(ctx, EVP_md5(), nullptr);
        EVP_DigestUpdate(ctx, str.c_str(), str.size());
        EVP_DigestFinal_ex(ctx, digest, &digest_len);
        EVP_MD_CTX_free(ctx);
        stringstream ss;
        for (unsigned int i = 0; i < digest_len; ++i)
            ss << hex << setw(2) << setfill('0') << (int)digest[i];
        return ss.str();
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