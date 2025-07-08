#ifndef CACHE_H
#define CACHE_H

#include <string>
#include <vector>

// Nodo de la cache
struct NodoCache {
    std::string clave;               // Consulta normalizada
    std::vector<int> resultados;     // IDs de documentos resultado
    NodoCache* prev;                 // Anterior en LRU
    NodoCache* next;                 // Siguiente en LRU
};

// Entrada de la tabla hash
struct EntradaHash {
    std::string clave;
    NodoCache* nodo;                 // Apunta al nodo en la lista LRU
    EntradaHash* siguiente;          // Para manejar colisiones
};

// Tabla hash
struct TablaHash {
    std::vector<EntradaHash*> buckets;   // Buckets de hash
    int numBuckets;                      // Cantidad de buckets
};

// Cache principal
struct Cache {
    int capacidad;
    int size;
    TablaHash tabla;
    NodoCache* head;                 // Más recientemente usado
    NodoCache* tail;                 // Menos recientemente usado
};

// Funciones
void inicializarCache(Cache& cache, int capacidad, int numBuckets);
bool buscarCache(Cache& cache, const std::string& clave, std::vector<int>& resultados);
void insertarCache(Cache& cache, const std::string& clave, const std::vector<int>& resultados);
void liberarCache(Cache& cache);
void mostrarEstadoCache(const Cache& cache);
unsigned int hashFunction(const std::string& clave, int numBuckets);

#endif




