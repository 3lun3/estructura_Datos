#include <cmath>
#include <iostream>
#include <iomanip>
#include "grafo.h"

using namespace std;

void inicializarGrafo(Grafo& grafo, int maxNodos) {
    grafo.numNodos = 0;
    grafo.numAristas = 0;
    grafo.ids = new int[maxNodos];
    grafo.pageRank = new double[maxNodos];
    grafo.adyacencia = new int*[maxNodos];
    for (int i = 0; i < maxNodos; ++i) {
        grafo.adyacencia[i] = new int[maxNodos]();
    }
}

int obtenerIndice(Grafo& grafo, int id) {
    for (int i = 0; i < grafo.numNodos; ++i)
        if (grafo.ids[i] == id)
            return i;

    grafo.ids[grafo.numNodos] = id;
    return grafo.numNodos++;
}

void agregarArista(Grafo& grafo, int id1, int id2) {
    int i = obtenerIndice(grafo, id1);
    int j = obtenerIndice(grafo, id2);

    if (!grafo.adyacencia[i][j]) {
        grafo.adyacencia[i][j] = 1;
        grafo.adyacencia[j][i] = 1;
        grafo.numAristas++;
    }
}

int calcularPageRank(Grafo& grafo, int maxIter, double damping) {
    int n = grafo.numNodos;
    double* nuevo = new double[n];
    double* anterior = new double[n];
    const double epsilon = 1e-6;

    // Inicialización uniforme
    for (int i = 0; i < n; ++i) {
        grafo.pageRank[i] = 1.0 / n;
        anterior[i] = grafo.pageRank[i];
    }

    int iter;
    for (iter = 1; iter <= maxIter; ++iter) {
        for (int i = 0; i < n; ++i) {
            nuevo[i] = (1.0 - damping) / n;

            for (int j = 0; j < n; ++j) {
                if (grafo.adyacencia[j][i]) {
                    int grado = 0;
                    for (int k = 0; k < n; ++k)
                        if (grafo.adyacencia[j][k]) grado++;

                    if (grado > 0)
                        nuevo[i] += damping * grafo.pageRank[j] / grado;
                }
            }
        }

        // Calcular la diferencia total (delta)
        double delta = 0.0;
        for (int i = 0; i < n; ++i) {
            delta += fabs(nuevo[i] - anterior[i]);
            anterior[i] = nuevo[i];
        }

        // Copiar al vector original
        for (int i = 0; i < n; ++i)
            grafo.pageRank[i] = nuevo[i];

        if (delta < epsilon) break;  // ✅ Convergió
    }

    delete[] nuevo;
    delete[] anterior;
    return iter;
}

void liberarGrafo(Grafo& grafo) {
    for (int i = 0; i < grafo.numNodos; ++i)
        delete[] grafo.adyacencia[i];
    delete[] grafo.adyacencia;
    delete[] grafo.ids;
    delete[] grafo.pageRank;
}




