#include "cache.h"
#include <iostream>

// Inicializa la cache y la tabla hash
void inicializarCache(Cache& cache, int capacidad, int numBuckets) {
    cache.capacidad = capacidad;
    cache.size = 0;
    cache.head = nullptr;
    cache.tail = nullptr;
    cache.tabla.numBuckets = numBuckets;
    cache.tabla.buckets.resize(numBuckets, nullptr);
}

//hash (suma de caracteres)
unsigned int hashFunction(const std::string& clave, int numBuckets) {
    unsigned int hash = 0;
    for (char c : clave) {
        hash += c;
    }
    return hash % numBuckets;
}

// Mueve un nodo al frente (más recientemente usado)
void moverAlFrente(Cache& cache, NodoCache* nodo) {
    if (cache.head == nodo) return;

    // Desconectar
    if (nodo->prev) nodo->prev->next = nodo->next;
    if (nodo->next) nodo->next->prev = nodo->prev;
    if (cache.tail == nodo) cache.tail = nodo->prev;

    // Insertar al frente
    nodo->prev = nullptr;
    nodo->next = cache.head;
    if (cache.head) cache.head->prev = nodo;
    cache.head = nodo;

    if (!cache.tail) cache.tail = nodo;
}

// Busca en la cache
bool buscarCache(Cache& cache, const std::string& clave, std::vector<int>& resultados) {
    unsigned int index = hashFunction(clave, cache.tabla.numBuckets);
    EntradaHash* entry = cache.tabla.buckets[index];

    while (entry) {
        if (entry->clave == clave) {
            resultados = entry->nodo->resultados;
            moverAlFrente(cache, entry->nodo);
            return true;
        }
        entry = entry->siguiente;
    }
    return false;
}

// Inserta en la cache
void insertarCache(Cache& cache, const std::string& clave, const std::vector<int>& resultados) {
    if (cache.size >= cache.capacidad) {
        // si la cache está llena se elimina el elemento LRU (que es la cola de la lista)
        NodoCache* lru = cache.tail;

        unsigned int idx = hashFunction(lru->clave, cache.tabla.numBuckets);
        EntradaHash* prev = nullptr;
        EntradaHash* entry = cache.tabla.buckets[idx];

        while (entry) {
            // se actualiza la lista y se libera la memoria
            if (entry->clave == lru->clave) {
                if (prev) prev->siguiente = entry->siguiente;
                else cache.tabla.buckets[idx] = entry->siguiente;
                delete entry;
                break;
            }
            prev = entry;
            entry = entry->siguiente;
        }

        if (lru->prev) lru->prev->next = nullptr;
        cache.tail = lru->prev;
        delete lru;
        cache.size--;
    }

     // se crea el nuevo nodo y se inserta al frente (el más reciente)
    NodoCache* nuevo = new NodoCache{clave, resultados, nullptr, cache.head};
    if (cache.head) cache.head->prev = nuevo;
    cache.head = nuevo;
    if (!cache.tail) cache.tail = nuevo;

    // Insertar en hash
    unsigned int index = hashFunction(clave, cache.tabla.numBuckets);
    EntradaHash* nuevaEntrada = new EntradaHash{clave, nuevo, cache.tabla.buckets[index]};
    cache.tabla.buckets[index] = nuevaEntrada;

    cache.size++;
}

// Libera memoria
void liberarCache(Cache& cache) {
    NodoCache* actual = cache.head;
    while (actual) {
        NodoCache* temp = actual;
        actual = actual->next;
        delete temp;
    }

    for (auto bucket : cache.tabla.buckets) {
        while (bucket) {
            EntradaHash* temp = bucket;
            bucket = bucket->siguiente;
            delete temp;
        }
    }
}

// Muestra el estado final de la cache
void mostrarEstadoCache(const Cache& cache) {
    std::cout << "\n--- Estado final de la cache (MRU -> LRU) ---\n";
    NodoCache* actual = cache.head;
    while (actual) {
        std::cout << actual->clave << " -> ";
        actual = actual->next;
    }
    std::cout << "NULL\n";
}




