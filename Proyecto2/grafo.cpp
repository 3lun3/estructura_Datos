#include "grafo.h"
#include <iostream>
#include <vector>
#include <cmath> // Usamos abs para calcular el error entre los valores de PageRank

// Función para agregar una arista entre dos nodos
void agregarArista(Grafo& grafo, int origen, int destino) {
    // Si el nodo de origen no existe en el grafo, lo creamos con un pagerank de 0.0 y sin aristas
    if (grafo.nodos.find(origen) == grafo.nodos.end()) {
        grafo.nodos[origen] = {0.0, 0, nullptr}; // Lo creamos vacío
    }
    // Lo mismo para el nodo de destino
    if (grafo.nodos.find(destino) == grafo.nodos.end()) {
        grafo.nodos[destino] = {0.0, 0, nullptr}; // Lo creamos vacío
    }

    // Aquí se agrega la arista de origen a destino
    NodoAdyacente* actual = grafo.nodos[origen].listaAdyacencia; // Empezamos a recorrer la lista de adyacencia de origen
    bool encontrado = false; // Flag para saber si ya existe la arista
    while (actual) {
        if (actual->idDestino == destino) { // Si encontramos la arista, solo incrementamos el peso
            actual->peso++; // Como ya existe, solo le sumamos un peso
            encontrado = true; // Marcamos que la encontramos
            break;
        }
        actual = actual->siguiente; // Si no la encontramos, pasamos al siguiente nodo
    }

    // Si no encontramos la arista (es nueva), la añadimos
    if (!encontrado) {
        NodoAdyacente* nuevo = new NodoAdyacente{destino, 1, grafo.nodos[origen].listaAdyacencia}; // Creamos una nueva arista con peso 1
        grafo.nodos[origen].listaAdyacencia = nuevo; // La añadimos al principio de la lista
    }
    grafo.nodos[origen].gradoSalida++; // Aumentamos el grado de salida del nodo de origen

    // Ahoracomo es un grafo no dirigido, también agregamos la arista de destino a origen
    actual = grafo.nodos[destino].listaAdyacencia; // Recursivamente, repetimos para el destino
    encontrado = false; // Volvemos a resetear la bandera
    while (actual) {
        if (actual->idDestino == origen) { // Si ya existe la arista solo aumentamos el peso
            actual->peso++;
            encontrado = true;
            break;
        }
        actual = actual->siguiente; // Continuamos buscando
    }

    // Si no existe la arista de destino a origen, la creamos
    if (!encontrado) {
        NodoAdyacente* nuevo = new NodoAdyacente{origen, 1, grafo.nodos[destino].listaAdyacencia}; // La creamos
        grafo.nodos[destino].listaAdyacencia = nuevo; // Y la añadimos al principio de la lista de adyacencia
    }
    grafo.nodos[destino].gradoSalida++; // Aumentamos el grado de salida del nodo de destino
}
// Calcula el PageRank de todos los nodos en el grafo
// Se detiene cuando converge o cuando llega al máximo de iteraciones
int calcularPageRank(Grafo& grafo, int maxIter, double d, double tol) {
    int n = grafo.nodos.size(); // Número de nodos en el grafo
    if (n == 0) return 0; // Si no hay nodos, no tiene sentido calcular PageRank

    // Inicializamos el PageRank de todos los nodos con un valor uniforme
    for (auto& par : grafo.nodos) {
        par.second.pagerank = 1.0 / n; // Asignamos a cada nodo un valor inicial igual
    }

    int iter;
    for (iter = 0; iter < maxIter; ++iter) { // Recorremos hasta el máximo de iteraciones
        std::map<int, double> nuevosPR; // Aquí almacenamos el nuevo PageRank para cada nodo
        double sumaDangle = 0.0; // Variable para los nodos sin salida (que no aportan a otros)

        // Primero, calculamos la contribución de los nodos colgantes
        for (const auto& par : grafo.nodos) {
            if (par.second.gradoSalida == 0) {
                sumaDangle += par.second.pagerank; // Sumar el PageRank de los nodos sin salida
            }
        }

        // Ahora, calculamos el nuevo PageRank de cada nodo usando la fórmula
        for (const auto& par : grafo.nodos) {
            nuevosPR[par.first] = (1.0 - d) / n + d * sumaDangle / n; // Fórmula para calcular el PageRank
        }

        // Aquí es donde entran las conexiones: calculamos el PageRank basado en los vecinos
        for (const auto& par : grafo.nodos) {
            NodoAdyacente* vecino = par.second.listaAdyacencia; // Empezamos a recorrer la lista de adyacencia
            while (vecino) {
                nuevosPR[vecino->idDestino] += d * par.second.pagerank * (static_cast<double>(vecino->peso) / par.second.gradoSalida);
                // Sumamos la contribución de cada vecino al PageRank del nodo de destino
                vecino = vecino->siguiente; // Avanzamos al siguiente vecino
            }
        }

        double error = 0.0; // Variable para calcular el error total de la convergencia
        for (const auto& par : grafo.nodos) {
            error += std::abs(nuevosPR[par.first] - par.second.pagerank); // Sumamos las diferencias entre los PR actuales y los nuevos
        }

        // Actualizamos el PageRank de cada nodo con los nuevos valores
        for (auto& par : grafo.nodos) {
            par.second.pagerank = nuevosPR[par.first];
        }

        // Si el error es menor que la tolerancia, consideramos que ya ha convergido
        if (error < tol) {
            std::cout << "\nPageRank convergio en " << iter + 1 << " iteraciones.\n";
            return iter + 1; // Devolvemos el número real de iteraciones realizadas
        }
    }

    // Si llegamos al máximo de iteraciones sin converger, lo indicamos
    std::cout << "\nPageRank alcanzo el maximo de " << maxIter << " iteraciones.\n";
    return maxIter; // Retornamos el máximo de iteraciones si no hubo convergencia
}

// Función para mostrar las métricas del grafo
void mostrarMétricas(const Grafo& grafo, int consultasUsadas, int iteraciones) {
    long long aristas = 0; // Variable para contar el número de aristas
    for (const auto& par : grafo.nodos) {
        aristas += par.second.gradoSalida; // Sumamos el grado de salida de cada nodo (el número de aristas)
    }

    // Mostramos las métricas de construcción del grafo
    std::cout << "\n--- METRICAS DE CONSTRUCCION ---\n";
    std::cout << "Numero total de consultas usadas para construir el grafo: " << consultasUsadas << "\n";
    std::cout << "Numero de nodos (documentos) en el grafo: " << grafo.nodos.size() << "\n";
    std::cout << "Numero de aristas en el grafo: " << aristas / 2 << "\n"; // Dividimos por 2 porque el grafo es no dirigido
    std::cout << "Numero de iteraciones requeridas para converger: " << iteraciones << "\n"; 
}

