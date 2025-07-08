#ifndef INDEX_H
#define INDEX_H

#include <string>
#include <map>

// Nodo de la lista de documentos donde aparece un término
struct NodoDoc {
    int idDocumento;      // ID del documento
    int frecuencia;       // Cantidad de veces que el término aparece en ese documento
    NodoDoc* siguiente;   // Puntero al siguiente nodo de la lista
};

// Nodo de la lista principal de términos del índice invertido
struct NodoTermino {
    std::string termino;      // Palabra (clave del índice)
    NodoDoc* listaDocumentos; // Sublista de documentos donde aparece la palabra
    NodoTermino* siguiente;   // Puntero al siguiente término en la lista principal
};

// Estructura principal del índice
struct IndiceInvertido {
    NodoTermino* inicio; // Inicio de la lista de términos
};

void inicializarIndice(IndiceInvertido& indice); // Prepara el índice
void insertarTermino(IndiceInvertido& indice, const std::string& termino, int idDocumento); // Inserta término
NodoTermino* buscarTermino(IndiceInvertido& indice, const std::string& termino); // Busca término
void liberarIndice(IndiceInvertido& indice); // Libera toda la memoria usada

void mostrarIndiceConFrecuencia(const IndiceInvertido& indice);         // Muestra la frecuencia de cada término
void mostrarIndiceConIDsComprimidos(const IndiceInvertido& indice);     // Muestra los IDs comprimidos por diferencia

#endif


