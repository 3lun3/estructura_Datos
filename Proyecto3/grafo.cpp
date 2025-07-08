#include <cmath>
#include <iostream>
#include <iomanip>
#include "grafo.h"

using namespace std;

// funcion para preparar el grafo, lo dejamos listo y vacio
void inicializarGrafo(Grafo& grafo, int maxNodos) {
    grafo.numNodos = 0;
    grafo.numAristas = 0;
    // reservamos memoria para todo con new
    grafo.ids = new int[maxNodos];
    grafo.pageRank = new double[maxNodos];
    grafo.adyacencia = new int*[maxNodos];
    // la matriz de adyacencia la inicializamos en cero
    for (int i = 0; i < maxNodos; ++i) {
        grafo.adyacencia[i] = new int[maxNodos]();
    }
}

// esta funcion es para mapear el ID de un documento  a un indice del array (0, 1, 2, etc)
int obtenerIndice(Grafo& grafo, int id) {
    // revisa si ya existe
    for (int i = 0; i < grafo.numNodos; ++i)
        if (grafo.ids[i] == id)
            return i; // si lo encontramos, devolvemos su posicion

    // si no existe, lo agregamos al final
    grafo.ids[grafo.numNodos] = id;
    // y devolvemos su nueva posicion, aumentando el contador de nodos
    return grafo.numNodos++;
}

// para agregar una  arista entre dos documentos
void agregarArista(Grafo& grafo, int id1, int id2) {
    // primero obtenemos los indices de los arrays para cada ID
    int i = obtenerIndice(grafo, id1);
    int j = obtenerIndice(grafo, id2);

    // si no existia una arista la creamos
    if (!grafo.adyacencia[i][j]) {
        // ponemos un 1 en la matriz. como es no dirigido, lo hacemos en ambos sentidos (i,j) y (j,i)
        grafo.adyacencia[i][j] = 1;
        grafo.adyacencia[j][i] = 1;
        // aumentamos el contador de aristas
        grafo.numAristas++;
    }
}

// la funcion principal para calcular el pagerank
int calcularPageRank(Grafo& grafo, int maxIter, double damping) {
    int n = grafo.numNodos;
    // usamos dos arrays para guardar el PR nuevo y el anterior en cada iteracion
    double* nuevo = new double[n];
    double* anterior = new double[n];
    const double epsilon = 1e-6; // un valor chico para ver si ya convergio

    // empezamos dandole a todos los nodos el mismo puntaje
    for (int i = 0; i < n; ++i) {
        grafo.pageRank[i] = 1.0 / n;
        anterior[i] = grafo.pageRank[i];
    }

    int iter;
    // el bucle principal, itera hasta un maximo o hasta que converja
    for (iter = 1; iter <= maxIter; ++iter) {
        // calculamos el nuevo PR para cada nodo i
        for (int i = 0; i < n; ++i) {
            // esta es la parte base del algoritmo el (1-d)/N
            nuevo[i] = (1.0 - damping) / n;

            // ahora sumamos la contribucion de los otros nodos j que apuntan a i
            for (int j = 0; j < n; ++j) {
                // si j apunta a i...
                if (grafo.adyacencia[j][i]) {
                    // necesitamos saber cuantas flechas salen de j (su grado)
                    int grado = 0;
                    for (int k = 0; k < n; ++k)
                        if (grafo.adyacencia[j][k]) grado++;

                    // si tiene grado, sumamos su contribucion (PR de j / grado de j)
                    if (grado > 0)
                        nuevo[i] += damping * grafo.pageRank[j] / grado;
                }
            }
        }

        // despues de calcular todos los nuevos puntajes vemos cuanto cambiaron
        double delta = 0.0;
        for (int i = 0; i < n; ++i) {
            delta += fabs(nuevo[i] - anterior[i]);
            anterior[i] = nuevo[i]; // guardamos los nuevos valores para la proxima iteracion
        }

        // actualiza el PR oficial con los nuevos valores
        for (int i = 0; i < n; ++i)
            grafo.pageRank[i] = nuevo[i];

        // si el cambio es muy chico (menor a epsilon), paramos el bucle. ya convergio!
        if (delta < epsilon) break;
    }

    // liberamos la memoria que usamos para los calculos
    delete[] nuevo;
    delete[] anterior;
    // devolvemos en cuantas iteraciones termino
    return iter;
}

// liberar toda la memoria que pedimos con new para no tener leaks
void liberarGrafo(Grafo& grafo) {
    // primero liberamos cada fila de la matriz
    for (int i = 0; i < grafo.numNodos; ++i)
        delete[] grafo.adyacencia[i];
    // despues el array de punteros
    delete[] grafo.adyacencia;
    
    // y los otros dos arrays
    delete[] grafo.ids;
    delete[] grafo.pageRank;
}
