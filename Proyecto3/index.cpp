#include "index.h"
#include <iostream>
#include <vector>
#include <algorithm>

// para dejar el indice vacio al principio
void inicializarIndice(IndiceInvertido& indice) {
    indice.inicio = nullptr;
}

// funcion para meter un documento en la lista de un termino (posting list)
void insertarDocumento(NodoDoc*& lista, int idDocumento) {
    // recorre la lista de documentos de ese termino
    NodoDoc* actual = lista;
    while (actual != nullptr) {
        // si ya estaba el doc, solo le suma uno a la frecuencia y listo
        if (actual->idDocumento == idDocumento) {
            actual->frecuencia += 1;
            return;
        }
        actual = actual->siguiente;
    }

    // si no estaba, creamos un nodo nuevo para el doc
    NodoDoc* nuevo = new NodoDoc;
    nuevo->idDocumento = idDocumento;
    nuevo->frecuencia = 1; //la primera vez que aparece, frecuencia 1
    // y lo ponemos al principio de la lista
    nuevo->siguiente = lista;
    lista = nuevo;
}

// aqui insertamos un termino en el indice
void insertarTermino(IndiceInvertido& indice, const std::string& termino, int idDocumento) {
    // recorremos la lista principal de terminos para ver si ya existe
    NodoTermino* actual = indice.inicio;
    while (actual != nullptr) {
        // si ya existe el termino
        if (actual->termino == termino) {
            // solo llamamos a insertarDocumento para que agregue el doc a su lista
            insertarDocumento(actual->listaDocumentos, idDocumento);
            return;
        }
        actual = actual->siguiente;
    }

    // si no existe el termino lo creamos
    NodoTermino* nuevoTermino = new NodoTermino;
    nuevoTermino->termino = termino;
    nuevoTermino->listaDocumentos = nullptr; // su lista de docs empieza vacia
    // lo ponemos al principio de la lista de terminos
    nuevoTermino->siguiente = indice.inicio;
    indice.inicio = nuevoTermino;

    // y le agregamos el documento a su nueva lista
    insertarDocumento(nuevoTermino->listaDocumentos, idDocumento);
}

// una funcion simple para buscar una palabra en el indice
NodoTermino* buscarTermino(IndiceInvertido& indice, const std::string& termino) {
    NodoTermino* actual = indice.inicio;
    while (actual != nullptr) {
        // si la encontramos devolvemos el puntero al nodo si no, null
        if (actual->termino == termino) return actual;
        actual = actual->siguiente;
    }
    return nullptr;
}

// funcion para imprimir el indice mostrando la frecuencia de cada palabra en cada doc
// para debuggear mas que nada
void mostrarIndiceConFrecuencia(const IndiceInvertido& indice) {
    NodoTermino* actual = indice.inicio;
    while (actual != nullptr) {
        std::cout << actual->termino << " -> ";
        NodoDoc* doc = actual->listaDocumentos;
        while (doc != nullptr) {
            std::cout << "[doc" << doc->idDocumento << ": " << doc->frecuencia << "] ";
            doc = doc->siguiente;
        }
        std::cout << std::endl;
        actual = actual->siguiente;
    }
}

// otra forma de mostrar el indice usando compresion por diferencia
void mostrarIndiceConIDsComprimidos(const IndiceInvertido& indice) {
    NodoTermino* actual = indice.inicio;
    while (actual != nullptr) {
        std::cout << actual->termino << " -> ";
        std::vector<int> ids;

        // juntamos todos los ids en un vector para poder ordenarlos
        NodoDoc* doc = actual->listaDocumentos;
        while (doc != nullptr) {
            ids.push_back(doc->idDocumento);
            doc = doc->siguiente;
        }

        // los ordenamos de menor a mayor
        std::sort(ids.begin(), ids.end());

        if (!ids.empty()) {
            std::cout << "[" << ids[0]; // imprimimos el primer id
            // y despues solo imprimimos la diferencia con el anterior. para ahorrar espacio
            for (size_t i = 1; i < ids.size(); ++i) {
                std::cout << ",+" << (ids[i] - ids[i - 1]);
            }
            std::cout << "]";
        }

        std::cout << std::endl;
        actual = actual->siguiente;
    }
}

// funcion importantisima para liberar toda la memoria del indice
void liberarIndice(IndiceInvertido& indice) {
    // recorremos cada termino
    NodoTermino* actual = indice.inicio;
    while (actual != nullptr) {
        // y para cada termino, recorremos su lista de documentos y los borramos uno por uno
        NodoDoc* doc = actual->listaDocumentos;
        while (doc != nullptr) {
            NodoDoc* tempDoc = doc;
            doc = doc->siguiente;
            delete tempDoc;
        }
        
        // una vez borrada la lista de docs, borramos el nodo del termino
        NodoTermino* tempTerm = actual;
        actual = actual->siguiente;
        delete tempTerm;
    }
    // al final, el indice queda vacio
    indice.inicio = nullptr;
}
