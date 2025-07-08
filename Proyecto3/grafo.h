#ifndef GRAFO_H
#define GRAFO_H

struct Grafo {
    int numNodos;           // Cantidad de nodos actuales
    int* ids;               // IDs de los documentos
    double* pageRank;       // Valores de PageRank para cada nodo
    int** adyacencia;       // Matriz de adyacencia (grafo no dirigido)
    int numAristas;         // Contador de aristas reales
};

// Inicializa el grafo con un número máximo de nodos
void inicializarGrafo(Grafo& grafo, int maxNodos);

// Agrega una arista entre dos nodos, si no existe
void agregarArista(Grafo& grafo, int id1, int id2);

// Calcula PageRank y retorna la cantidad de iteraciones hasta convergencia o maxIter
int calcularPageRank(Grafo& grafo, int maxIter, double damping);

// Libera la memoria del grafo
void liberarGrafo(Grafo& grafo);

#endif




