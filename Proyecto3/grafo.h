#ifndef GRAFO_H
#define GRAFO_H

// para que no se incluya dos veces y de error de compilacion

// La estructura principal del grafo, aqui guardamos todo
struct Grafo {
    int numNodos;           // cuantos nodos tenemos ahora mismo
    int* ids;               // el array que guarda los IDs de los docs
    double* pageRank;       // el array para los puntajes de pagerank
    int** adyacencia;       // la matriz para ver que nodo se conecta con que nodo
    int numAristas;         // para contar cuantas aristas hay en el grafo
};



// para preparar el grafo
void inicializarGrafo(Grafo& grafo, int maxNodos);

// para conectar dos nodos
void agregarArista(Grafo& grafo, int id1, int id2);

// la funcion del pagerank devuelve las iteraciones que tomo
int calcularPageRank(Grafo& grafo, int maxIter, double damping);

// para borrar todo al final y que no queden memory leaks
void liberarGrafo(Grafo& grafo);

#endif // GRAFO_H
