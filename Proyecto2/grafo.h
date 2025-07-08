#ifndef GRAFO_H
#define GRAFO_H

#include <map>  // Usamos map para almacenar los nodos y sus respectivas propiedades
#include <vector> // Usamos vector si necesitamos listas dinámicas, en este caso para adyacencia

// Representa una conexión a un nodo vecino
// Aquí almacenamos el id del nodo destino, el peso de la conexión y el siguiente nodo en la lista de adyacencia
struct NodoAdyacente {
    int idDestino;      // El identificador del nodo destino al que se conecta
    int peso;           // El peso de la arista
    NodoAdyacente* siguiente; // Puntero al siguiente nodo en la lista de adyacencia (si es que existe)
};

// Representa un documento en el grafo
// Cada documento tiene su propio PageRank, grado de salida y una lista de nodos a los que está conectado
struct NodoGrafo {
    double pagerank;    // Valor de PageRank para este nodo (documento)
    int gradoSalida;    // Suma de los pesos de las aristas salientes (es decir, la cantidad de conexiones hacia otros nodos)
    NodoAdyacente* listaAdyacencia; // Puntero a la lista de nodos a los que este nodo está conectado
};

// Estructura principal del grafo
// Utilizamos un mapa donde la clave es el id del nodo y el valor es la información del nodo (NodoGrafo)
struct Grafo {
    std::map<int, NodoGrafo> nodos; // Cada nodo del grafo es accesible por su id, que es el índice del mapa
};

// Prototipos de funciones que se usan para operar sobre el grafo
// La función 'agregarArista' se usa para conectar dos nodos (documentos) con una arista
void agregarArista(Grafo& grafo, int origen, int destino);

// Calcula el PageRank de todos los nodos en el grafo
// Se ejecuta hasta que el algoritmo converge o se alcanzan el máximo de iteraciones
int calcularPageRank(Grafo& grafo, int maxIter, double d, double tol);

// Muestra las métricas del grafo: número de nodos, aristas, iteraciones, etc.
void mostrarMétricas(const Grafo& grafo, int consultasUsadas, int iteraciones);

#endif
