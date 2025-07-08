#ifndef INDEX_H
#define INDEX_H

#include <string>
#include <map>

// para que no se compile dos veces el mismo archivo

// este es el nodo para la lista de documentos (la posting list)
struct NodoDoc {
    int idDocumento;        // el id del doc
    int frecuencia;         // cuantas veces sale la palabra en ese doc
    NodoDoc* siguiente;     // puntero al siguiente doc de la lista
};

// el nodo para la lista principal, la de las palabras
struct NodoTermino {
    std::string termino;        // la palabra en si
    NodoDoc* listaDocumentos;   // un puntero a la lista de todos los docs donde sale esta palabra
    NodoTermino* siguiente;     // puntero a la siguiente palabra del indice
};

// la estructura principal de todo el indice
struct IndiceInvertido {
    NodoTermino* inicio; // solo necesitamos saber donde empieza la lista de terminos
};


void inicializarIndice(IndiceInvertido& indice);                 // para empezar el indice de cero
void insertarTermino(IndiceInvertido& indice, const std::string& termino, int idDocumento); // para meter una palabra y el doc donde salio
NodoTermino* buscarTermino(IndiceInvertido& indice, const std::string& termino); // para buscar una palabra
void liberarIndice(IndiceInvertido& indice);                     // para borrar todo y no dejar fugas de memoria

// funciones para mostrar el indice, mas que nada para pruebas
void mostrarIndiceConFrecuencia(const IndiceInvertido& indice);
void mostrarIndiceConIDsComprimidos(const IndiceInvertido& indice);

#endif // INDEX_H
