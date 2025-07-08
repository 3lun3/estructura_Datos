#include "index.h"
#include <iostream>
#include <vector>
#include <algorithm> // Necesario para usar std::sort en la compresión de IDs

// Inicializa el índice vacío. Simplemente se pone el inicio del índice en nullptr
void inicializarIndice(IndiceInvertido& indice) {
    indice.inicio = nullptr;  // El índice empieza vacío, sin términos.
}

// Inserta un documento en la lista asociada a un término.
// Si el documento ya existe para ese término, incrementa la frecuencia. Si no, lo crea.
void insertarDocumento(NodoDoc*& lista, int idDocumento) {
    NodoDoc* actual = lista; // Empezamos a recorrer la lista de documentos
    while (actual != nullptr) {
        if (actual->idDocumento == idDocumento) { 
            actual->frecuencia += 1; // Si el documento ya está, simplemente aumentamos la frecuencia
            return; // Salimos de la función
        }
        actual = actual->siguiente; // Si no encontramos el documento, seguimos al siguiente
    }

    // Si no encontramos el documento, significa que no está en la lista. Así que lo agregamos.
    NodoDoc* nuevo = new NodoDoc;  // Creamos un nuevo nodo para el documento
    nuevo->idDocumento = idDocumento;
    nuevo->frecuencia = 1;  // La frecuencia inicial será 1
    nuevo->siguiente = lista;  // Lo insertamos al principio de la lista
    lista = nuevo;  // Ahora el nuevo nodo es el primer documento de la lista
}

// Inserta un término en el índice, o actualiza el término si ya existe
// Si el término ya está en el índice, simplemente agrega el documento a su lista de documentos
void insertarTermino(IndiceInvertido& indice, const std::string& termino, int idDocumento) {
    NodoTermino* actual = indice.inicio;  // Empezamos a recorrer la lista de términos
    while (actual != nullptr) {
        if (actual->termino == termino) {
            insertarDocumento(actual->listaDocumentos, idDocumento); // Si el término ya está, solo agregamos el documento
            return;
        }
        actual = actual->siguiente;  // Si no lo encontramos, seguimos al siguiente término
    }

    // Si el término no existe en la lista, lo creamos.
    NodoTermino* nuevoTermino = new NodoTermino;  // Creamos un nuevo nodo para el término
    nuevoTermino->termino = termino;
    nuevoTermino->listaDocumentos = nullptr;  // Inicialmente no tiene documentos
    nuevoTermino->siguiente = indice.inicio;  // Lo agregamos al principio de la lista de términos
    indice.inicio = nuevoTermino;  // Ahora el nuevo término es el primero en la lista

    // Ahora que tenemos el término, agregamos el documento a su lista de documentos
    insertarDocumento(nuevoTermino->listaDocumentos, idDocumento);  // Insertamos el documento en la lista del nuevo término
}

// Busca un término en el índice. Si lo encuentra, retorna el nodo que lo contiene.
NodoTermino* buscarTermino(IndiceInvertido& indice, const std::string& termino) {
    NodoTermino* actual = indice.inicio; // Comenzamos a recorrer la lista de términos
    while (actual != nullptr) {
        if (actual->termino == termino) return actual;  // Si encontramos el término, lo devolvemos
        actual = actual->siguiente;  // Si no, seguimos al siguiente término
    }
    return nullptr;  // Si no encontramos el término, devolvemos nullptr
}

// Muestra el índice mostrando la frecuencia de cada término en los documentos
void mostrarIndiceConFrecuencia(const IndiceInvertido& indice) {
    NodoTermino* actual = indice.inicio; // Empezamos con el primer término
    while (actual != nullptr) {
        std::cout << actual->termino << " → ";  // Mostramos el término
        NodoDoc* doc = actual->listaDocumentos; // Empezamos a recorrer la lista de documentos
        while (doc != nullptr) {
            std::cout << "[doc" << doc->idDocumento << ": " << doc->frecuencia << "] ";  // Mostramos el documento y su frecuencia
            doc = doc->siguiente;  // Avanzamos al siguiente documento
        }
        std::cout << std::endl;  // Imprimimos una nueva línea para separar los términos
        actual = actual->siguiente;  // Pasamos al siguiente término
    }
}

// Muestra el índice usando compresión por diferencia entre IDs
// Esto es útil para ahorrar espacio al mostrar los documentos asociados a un término
void mostrarIndiceConIDsComprimidos(const IndiceInvertido& indice) {
    NodoTermino* actual = indice.inicio;  // Empezamos con el primer término
    while (actual != nullptr) {
        std::cout << actual->termino << " → ";  // Mostramos el término
        std::vector<int> ids;  // Vector para almacenar los IDs de los documentos

        // Recolectamos todos los IDs de los documentos asociados a este término
        NodoDoc* doc = actual->listaDocumentos;
        while (doc != nullptr) {
            ids.push_back(doc->idDocumento);  // Guardamos el ID de cada documento
            doc = doc->siguiente;  // Avanzamos al siguiente documento
        }

        // Ordenamos los IDs para aplicar la compresión por diferencia
        std::sort(ids.begin(), ids.end());  // Ordenamos los IDs en orden ascendente

        if (!ids.empty()) {
            std::cout << "[" << ids[0];  // Mostramos el primer ID
            // Luego mostramos las diferencias entre IDs consecutivos
            for (size_t i = 1; i < ids.size(); ++i) {
                std::cout << ",+" << (ids[i] - ids[i - 1]);  // Diferencia entre el ID actual y el anterior
            }
            std::cout << "]";  // Cerramos la lista de IDs comprimidos
        }

        std::cout << std::endl;  // Imprimimos una nueva línea para separar los términos
        actual = actual->siguiente;  // Pasamos al siguiente término
    }
}

// Libera toda la memoria dinámica usada por el índice
// Es importante liberar la memoria cuando ya no necesitamos los datos
void liberarIndice(IndiceInvertido& indice) {
    NodoTermino* actual = indice.inicio;  // Empezamos con el primer término
    while (actual != nullptr) {
        NodoDoc* doc = actual->listaDocumentos;  // Obtenemos la lista de documentos del término actual
        while (doc != nullptr) {
            NodoDoc* tempDoc = doc;  // Guardamos el nodo actual para liberarlo después
            doc = doc->siguiente;  // Avanzamos al siguiente documento
            delete tempDoc;  // Liberamos la memoria del documento actual
        }
        NodoTermino* tempTerm = actual;  // Guardamos el nodo actual de término
        actual = actual->siguiente;  // Avanzamos al siguiente término
        delete tempTerm;  // Liberamos la memoria del término actual
    }
    indice.inicio = nullptr;  // Al final, el índice queda vacío
}



