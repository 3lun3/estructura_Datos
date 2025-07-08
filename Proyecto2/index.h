#ifndef INDEX_H
#define INDEX_H

#include <string> 
#include <map>    // Para usar mapas, aunque no lo estamos usando directamente en esta cabecera

// Nodo de la lista de documentos donde aparece un término
// Cada nodo representa un documento que contiene un término específico
struct NodoDoc {
    int idDocumento;      // El ID único del documento que contiene el término
    int frecuencia;       // La cantidad de veces que el término aparece en ese documento
    NodoDoc* siguiente;   // Puntero al siguiente documento en la lista de documentos
};

// Nodo de la lista principal de términos del índice invertido
// Cada nodo representa un término (palabra) que aparece en algún documento
struct NodoTermino {
    std::string termino;      
    NodoDoc* listaDocumentos; // Puntero a la lista de documentos donde aparece el término
    NodoTermino* siguiente;   // Puntero al siguiente término en el índice invertido
};

// Estructura principal del índice invertido
// Usamos un índice invertido para mapear términos a documentos
struct IndiceInvertido {
    NodoTermino* inicio; // Puntero al primer término del índice (la lista comienza aquí)
};

// Funciones del índice invertido

// Inicializa el índice vacío, poniendo el inicio del índice a nullptr
void inicializarIndice(IndiceInvertido& indice); // Prepara el índice para ser usado

// Inserta un término en el índice invertido, o lo actualiza si ya existe
void insertarTermino(IndiceInvertido& indice, const std::string& termino, int idDocumento); // Inserta un término en el índice

// Busca un término en el índice. Si lo encuentra, devuelve el nodo correspondiente
NodoTermino* buscarTermino(IndiceInvertido& indice, const std::string& termino); // Busca un término

// Libera toda la memoria dinámica utilizada por el índice, eliminando los nodos
void liberarIndice(IndiceInvertido& indice); // Libera todos los nodos del índice

// Muestra el índice mostrando la frecuencia de cada término en los documentos
void mostrarIndiceConFrecuencia(const IndiceInvertido& indice); // Muestra la frecuencia de los términos en los documentos

// Muestra el índice usando compresión por diferencia entre los IDs de los documentos
void mostrarIndiceConIDsComprimidos(const IndiceInvertido& indice); // Muestra los IDs comprimidos por la diferencia entre ellos

#endif


