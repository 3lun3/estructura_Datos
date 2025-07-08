#include "index.h"
#include <iostream>
#include <vector>
#include <algorithm>

// Inicializa el índice vacío
void inicializarIndice(IndiceInvertido& indice) {
    indice.inicio = nullptr;
}

// Inserta un documento en la lista asociada a un término
// Si ya existe, aumenta la frecuencia; si no, lo crea
void insertarDocumento(NodoDoc*& lista, int idDocumento) {
    NodoDoc* actual = lista;
    while (actual != nullptr) {
        if (actual->idDocumento == idDocumento) {
            actual->frecuencia += 1; // Documento ya existe entonces aumenta frecuencia
            return;
        }
        actual = actual->siguiente;
    }

    // Documento no estaba entonces crea nuevo nodo con frecuencia 1
    NodoDoc* nuevo = new NodoDoc;
    nuevo->idDocumento = idDocumento;
    nuevo->frecuencia = 1;
    nuevo->siguiente = lista;
    lista = nuevo;
}

// Inserta un término en el índice, o actualiza si ya existe
void insertarTermino(IndiceInvertido& indice, const std::string& termino, int idDocumento) {
    NodoTermino* actual = indice.inicio;
    while (actual != nullptr) {
        if (actual->termino == termino) {
            insertarDocumento(actual->listaDocumentos, idDocumento); // Agrega o actualiza documento
            return;
        }
        actual = actual->siguiente;
    }

    // Si no existe el término, lo crea
    NodoTermino* nuevoTermino = new NodoTermino;
    nuevoTermino->termino = termino;
    nuevoTermino->listaDocumentos = nullptr;
    nuevoTermino->siguiente = indice.inicio;
    indice.inicio = nuevoTermino;

    insertarDocumento(nuevoTermino->listaDocumentos, idDocumento);
}

// Busca un término en el índice
NodoTermino* buscarTermino(IndiceInvertido& indice, const std::string& termino) {
    NodoTermino* actual = indice.inicio;
    while (actual != nullptr) {
        if (actual->termino == termino) return actual;
        actual = actual->siguiente;
    }
    return nullptr;
}

// Muestra el índice mostrando la frecuencia de cada término en los documentos
void mostrarIndiceConFrecuencia(const IndiceInvertido& indice) {
    NodoTermino* actual = indice.inicio;
    while (actual != nullptr) {
        std::cout << actual->termino << " → ";
        NodoDoc* doc = actual->listaDocumentos;
        while (doc != nullptr) {
            std::cout << "[doc" << doc->idDocumento << ": " << doc->frecuencia << "] ";
            doc = doc->siguiente;
        }
        std::cout << std::endl;
        actual = actual->siguiente;
    }
}

// Muestra el índice usando compresión por diferencia entre IDs
void mostrarIndiceConIDsComprimidos(const IndiceInvertido& indice) {
    NodoTermino* actual = indice.inicio;
    while (actual != nullptr) {
        std::cout << actual->termino << " → ";
        std::vector<int> ids;

        // Recolectar todos los IDs
        NodoDoc* doc = actual->listaDocumentos;
        while (doc != nullptr) {
            ids.push_back(doc->idDocumento);
            doc = doc->siguiente;
        }

        // Ordenar para aplicar compresión por diferencia
        std::sort(ids.begin(), ids.end());

        if (!ids.empty()) {
            std::cout << "[" << ids[0]; // Primer ID
            for (size_t i = 1; i < ids.size(); ++i) {
                std::cout << ",+" << (ids[i] - ids[i - 1]); // Diferencia con anterior
            }
            std::cout << "]";
        }

        std::cout << std::endl;
        actual = actual->siguiente;
    }
}

// Libera toda la memoria dinámica usada por el índice
void liberarIndice(IndiceInvertido& indice) {
    NodoTermino* actual = indice.inicio;
    while (actual != nullptr) {
        NodoDoc* doc = actual->listaDocumentos;
        while (doc != nullptr) {
            NodoDoc* tempDoc = doc;
            doc = doc->siguiente;
            delete tempDoc;
        }
        NodoTermino* tempTerm = actual;
        actual = actual->siguiente;
        delete tempTerm;
    }
    indice.inicio = nullptr;
}


