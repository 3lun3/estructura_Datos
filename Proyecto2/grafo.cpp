#include "grafo.h"
#include <iostream>
#include <vector>
#include <cmath> // Para std::abs

// Agrega una arista (o incrementa su peso) entre dos nodos
void agregarArista(Grafo& grafo, int origen, int destino) {
    // Asegurarse de que ambos nodos existen en el grafo
    if (grafo.nodos.find(origen) == grafo.nodos.end()) {
        grafo.nodos[origen] = {0.0, 0, nullptr};
    }
    if (grafo.nodos.find(destino) == grafo.nodos.end()) {
        grafo.nodos[destino] = {0.0, 0, nullptr};
    }

    // Añadir arista de origen a destino
    NodoAdyacente* actual = grafo.nodos[origen].listaAdyacencia;
    bool encontrado = false;
    while (actual) {
        if (actual->idDestino == destino) {
            actual->peso++;
            encontrado = true;
            break;
        }
        actual = actual->siguiente;
    }
    if (!encontrado) {
        NodoAdyacente* nuevo = new NodoAdyacente{destino, 1, grafo.nodos[origen].listaAdyacencia};
        grafo.nodos[origen].listaAdyacencia = nuevo;
    }
    grafo.nodos[origen].gradoSalida++;

    // Añadir arista de destino a origen (grafo no dirigido)
    actual = grafo.nodos[destino].listaAdyacencia;
    encontrado = false;
    while (actual) {
        if (actual->idDestino == origen) {
            actual->peso++;
            encontrado = true;
            break;
        }
        actual = actual->siguiente;
    }
    if (!encontrado) {
        NodoAdyacente* nuevo = new NodoAdyacente{origen, 1, grafo.nodos[destino].listaAdyacencia};
        grafo.nodos[destino].listaAdyacencia = nuevo;
    }
    grafo.nodos[destino].gradoSalida++;
}

// --- CAMBIO: La función ahora implementa la convergencia ---
// Calcula el PageRank de los nodos del grafo.
// Se detiene cuando converge o alcanza el máximo de iteraciones.
int calcularPageRank(Grafo& grafo, int maxIter, double d, double tol) {
    int n = grafo.nodos.size();
    if (n == 0) return 0;

    // Inicializar PageRank
    for (auto& par : grafo.nodos) {
        par.second.pagerank = 1.0 / n;
    }

    int iter;
    for (iter = 0; iter < maxIter; ++iter) {
        std::map<int, double> nuevosPR;
        double sumaDangle = 0.0;

        // Calcular contribución de nodos "colgantes" (sin salida)
        for (const auto& par : grafo.nodos) {
            if (par.second.gradoSalida == 0) {
                sumaDangle += par.second.pagerank;
            }
        }

        // Calcular nuevos PR
        for (const auto& par : grafo.nodos) {
            nuevosPR[par.first] = (1.0 - d) / n + d * sumaDangle / n;
        }

        for (const auto& par : grafo.nodos) {
            NodoAdyacente* vecino = par.second.listaAdyacencia;
            while (vecino) {
                nuevosPR[vecino->idDestino] += d * par.second.pagerank * (static_cast<double>(vecino->peso) / par.second.gradoSalida);
                vecino = vecino->siguiente;
            }
        }

        // --- CAMBIO: Comprobar convergencia ---
        double error = 0.0;
        for (const auto& par : grafo.nodos) {
            error += std::abs(nuevosPR[par.first] - par.second.pagerank);
        }

        // Actualizar los PR en el grafo
        for (auto& par : grafo.nodos) {
            par.second.pagerank = nuevosPR[par.first];
        }

        if (error < tol) {
            std::cout << "\nPageRank convergio en " << iter + 1 << " iteraciones.\n";
            return iter + 1; // Devolver número de iteraciones real
        }
    }

    std::cout << "\nPageRank alcanzo el maximo de " << maxIter << " iteraciones.\n";
    return maxIter;
}


// Muestra las métricas del grafo
void mostrarMétricas(const Grafo& grafo, int consultasUsadas, int iteraciones) {
    long long aristas = 0;
    for (const auto& par : grafo.nodos) {
        aristas += par.second.gradoSalida;
    }

    std::cout << "\n--- METRICAS DE CONSTRUCCION ---\n";
    std::cout << "Numero total de consultas usadas para construir el grafo: " << consultasUsadas << "\n";
    std::cout << "Numero de nodos (documentos) en el grafo: " << grafo.nodos.size() << "\n";
    std::cout << "Numero de aristas en el grafo: " << aristas / 2 << "\n"; // Dividido por 2 porque es no dirigido
    std::cout << "Numero de iteraciones requeridas para converger: " << iteraciones << "\n";
}