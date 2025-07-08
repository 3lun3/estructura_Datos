#ifndef CACHE_H
#define CACHE_H

#include <string>
#include <vector>

// para evitar que se incluya el archivo mas de una vez
// esto es lo que va en la lista doblemente enlazada (la del LRU)
struct NodoCache {
    std::string clave;              // la consulta que se guardo
    std::vector<int> resultados;    // el vector con los ids de los docs
    NodoCache* prev;                // puntero al de atras
    NodoCache* next;                // puntero al de adelante
};

// esto es para la tabla hash, cada entrada en la tabla
struct EntradaHash {
    std::string clave;
    NodoCache* nodo;                //  un puntero al nodo de la lista, para encontrarlo rapido
    EntradaHash* siguiente;         // el puntero al siguiente por si hay colision
};

// La estructura de la tabla hash en si
struct TablaHash {
    std::vector<EntradaHash*> buckets;  // el array de buckets
    int numBuckets;                     // cuantos buckets tiene la tabla
};

// La estructura principal de la Cache
// juntamos la tabla hash y la lista del lru
struct Cache {
    int capacidad;      // cuantos elementos caben como maximo
    int size;           // cuantos elementos hay ahora
    TablaHash tabla;    // nuestra tabla hash
    NodoCache* head;    // la cabeza de la lista
    NodoCache* tail;    // la cola de la lista (el que borramos)
};

// prototipos de las funciones 

// para empezar la cache desde cero
void inicializarCache(Cache& cache, int capacidad, int numBuckets);
// busca algo en la cache, devuelve true si lo encuentra
bool buscarCache(Cache& cache, const std::string& clave, std::vector<int>& resultados);
// para meter algo nuevo
void insertarCache(Cache& cache, const std::string& clave, const std::vector<int>& resultados);
// para borrar todo y liberar memoria
void liberarCache(Cache& cache);
// una funcion para ver como esta la cache, para debug
void mostrarEstadoCache(const Cache& cache);
// la funcion que convierte el string a numero
unsigned int hashFunction(const std::string& clave, int numBuckets);

#endif // CACHE_H
