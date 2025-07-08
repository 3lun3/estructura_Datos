#ifndef GRAFO_H
#define GRAFO_H

#include <map>
#include <vector>

// Representa una conexión a un nodo vecino
struct NodoAdyacente {
    int idDestino;
    int peso;
    NodoAdyacente* siguiente;
};

// Representa un documento en el grafo
struct NodoGrafo {
    double pagerank;
    int gradoSalida; // Suma de pesos de aristas salientes
    NodoAdyacente* listaAdyacencia;
};

// Estructura principal del grafo
struct Grafo {
    std::map<int, NodoGrafo> nodos;
};

// Prototipos de funciones
void agregarArista(Grafo& grafo, int origen, int destino);
int calcularPageRank(Grafo& grafo, int maxIter, double d, double tol);
void mostrarMétricas(const Grafo& grafo, int consultasUsadas, int iteraciones);

#endif // GRAFO_H