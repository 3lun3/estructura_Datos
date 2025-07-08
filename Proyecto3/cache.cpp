#include "cache.h"
#include <iostream>

// funcion para inicializar la cache, poner todo en cero
void inicializarCache(Cache& cache, int capacidad, int numBuckets) {
    //le pasamos la capacidad y el numero de buckets para la tabla hash
    cache.capacidad = capacidad;
    cache.size = 0;
    
    // la cache empieza vacia, los punteros head y tail a nulo
    cache.head = nullptr;
    cache.tail = nullptr;
    
    // preparamos la tabla hash
    cache.tabla.numBuckets = numBuckets;
    cache.tabla.buckets.resize(numBuckets, nullptr);
}

// nuestra funcion de hash. es la simple de sumar los ascii
unsigned int hashFunction(const std::string& clave, int numBuckets) {
    unsigned int hash = 0;
    
    //recorremos la clave caracter por caracter y vamos sumando
    for (char c : clave) {
        hash += c;
    }
    
    // al final le aplicamos el modulo para que nos de un indice valido
    return hash % numBuckets;
}

// esta funcion mueve un nodo para el frente de la lista
// lo usamos para el LRU, asi el que usamos recien queda como el mas nuevo
void moverAlFrente(Cache& cache, NodoCache* nodo) {
    //si ya esta al frente, no hacemos nada
    if (cache.head == nodo) return;

    // aqui lo desconectamos de donde estaba
    if (nodo->prev) nodo->prev->next = nodo->next;
    if (nodo->next) nodo->next->prev = nodo->prev;

    // si era la cola, la cola ahora es el anterior
    if (cache.tail == nodo) cache.tail = nodo->prev;

    // y aca lo ponemos al principio
    nodo->prev = nullptr;
    nodo->next = cache.head;
    if (cache.head) cache.head->prev = nodo; //la antigua cabeza ahora apunta al nuevo
    cache.head = nodo;

    //si la lista estaba vacia, la cola tambien es este nodo
    if (!cache.tail) cache.tail = nodo;
}

// funcion para buscar en la cache. si lo encontramos es un HIT si no es un MISS
bool buscarCache(Cache& cache, const std::string& clave, std::vector<int>& resultados) {
    // primero calculamos el hash para saber en que bucket buscar
    unsigned int index = hashFunction(clave, cache.tabla.numBuckets);
    EntradaHash* entry = cache.tabla.buckets[index];

    //recorremos la lista de colisiones de ese bucket
    while (entry) {
        // si la clave es la misma, es un hit
        if (entry->clave == clave) {
            // devolvemos los resultados
            resultados = entry->nodo->resultados;
            // movemos el nodo al frente para actualizar el LRU
            moverAlFrente(cache, entry->nodo);
            return true;
        }
        // si no era seguimos buscando en la lista
        entry = entry->siguiente;
    }
    // si salimos del while es porque no estaba (miss)
    return false;
}

// funcion para meter algo nuevo en la cache
void insertarCache(Cache& cache, const std::string& clave, const std::vector<int>& resultados) {
    // primero vemos si la cache esta llena
    if (cache.size >= cache.capacidad) {
        //si esta llena hay que eliminar al mas viejo (el LRU) 
        NodoCache* lru = cache.tail;

        //borrarlo de la tabla hash
        unsigned int idx = hashFunction(lru->clave, cache.tabla.numBuckets);
        EntradaHash* prev = nullptr;
        EntradaHash* entry = cache.tabla.buckets[idx];

        while (entry) {
            //buscamos en la lista de colision y lo sacamos
            if (entry->clave == lru->clave) {
                if (prev) prev->siguiente = entry->siguiente;
                else cache.tabla.buckets[idx] = entry->siguiente;
                delete entry;
                break;
            }
            prev = entry;
            entry = entry->siguiente;
        }

        // y ahora lo borramos de la lista doblemente enlazada
        if (lru->prev) lru->prev->next = nullptr;
        cache.tail = lru->prev; //actualizamos el puntero tail
        delete lru; // liberamos la memoria del nodo
        cache.size--;
    }

    // metemos el nuevo
    // creamos el nodo y lo ponemos al principio (head)
    NodoCache* nuevo = new NodoCache{clave, resultados, nullptr, cache.head};
    if (cache.head) cache.head->prev = nuevo;
    cache.head = nuevo;
    if (!cache.tail) cache.tail = nuevo;

    // y lo metemos tambien en la tabla hash
    unsigned int index = hashFunction(clave, cache.tabla.numBuckets);
    EntradaHash* nuevaEntrada = new EntradaHash{clave, nuevo, cache.tabla.buckets[index]};
    cache.tabla.buckets[index] = nuevaEntrada;

    cache.size++;
}

// para que no hayan memory leaks, liberamos todo
void liberarCache(Cache& cache) {
    // primero la lista doble
    NodoCache* actual = cache.head;
    while (actual) {
        NodoCache* temp = actual;
        actual = actual->next;
        delete temp;
    }

    // y despues la tabla hash, bucket por bucket
    for (auto bucket : cache.tabla.buckets) {
        while (bucket) {
            EntradaHash* temp = bucket;
            bucket = bucket->siguiente;
            delete temp;
        }
    }
}

// funcion para ver como esta la cache, mas que nada para debuggear
void mostrarEstadoCache(const Cache& cache) {
    std::cout << "\n--- Estado de la cache (MRU -> LRU) ---\n";
    // imprime desde el mas nuevo (head) al mas viejo (tail)
    NodoCache* actual = cache.head;
    while (actual) {
        std::cout << "\"" << actual->clave << "\" -> ";
        actual = actual->next;
    }
    std::cout << "NULL\n";
}
