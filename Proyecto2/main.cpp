#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <iomanip>

#include "index.h"
#include "utils.h"
#include "grafo.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Uso: " << argv[0] << " <archivo_documentos> <archivo_consultas> <archivo_stopwords>\n";
        return 1;
    }

    std::string archivoDocumentos = argv[1];
    std::string archivoConsultas = argv[2];
    std::string archivoStopwords = argv[3];

    IndiceInvertido indice;
    inicializarIndice(indice);

    std::set<std::string> stopwords;
    cargarStopwords(archivoStopwords, stopwords);

    // Construir el índice
    std::ifstream documentos(archivoDocumentos);
    std::string linea;
    int idDoc = 1;

    int totalDocs = 0;
    std::ifstream contar(archivoDocumentos);
    while (std::getline(contar, linea)) totalDocs++;
    contar.close();

    std::cout << "Construyendo índice invertido...\n";
    while (std::getline(documentos, linea)) {
        size_t pos = linea.rfind("||");
        if (pos != std::string::npos) {
            std::string contenido = linea.substr(pos + 2);
            std::istringstream iss(contenido);
            std::string palabra;
            while (iss >> palabra) {
                std::string limpia = limpiarPalabra(palabra);
                if (!limpia.empty() && !esStopword(limpia, stopwords)) {
                    insertarTermino(indice, limpia, idDoc);
                }
            }
        }
        int progreso = (idDoc * 100) / totalDocs;
        std::cout << "\rProgreso índice: " << progreso << "%" << std::flush;
        idDoc++;
    }
    documentos.close();
    std::cout << "\nÍndice invertido construido.\n";

    // Construcción del grafo
    Grafo grafo;
    std::ifstream consultas(archivoConsultas);
    std::vector<std::string> todasConsultas;
    while (std::getline(consultas, linea)) {
        if (!linea.empty()) todasConsultas.push_back(linea);
    }
    consultas.close();

    int consultasUsadas = 0;
    auto startGrafo = std::chrono::steady_clock::now();

    std::cout << "Construyendo grafo de co-relevancia...\n";
    for (size_t i = 0; i < todasConsultas.size(); ++i) {
        std::string consulta = todasConsultas[i];
        std::istringstream iss(consulta);
        std::string palabra;
        std::vector<NodoDoc*> listas;

        while (iss >> palabra) {
            std::string limpia = limpiarPalabra(palabra);
            if (!limpia.empty() && !esStopword(limpia, stopwords)) {
                NodoTermino* nodo = buscarTermino(indice, limpia);
                if (nodo) listas.push_back(nodo->listaDocumentos);
            }
        }

        std::set<int> resultado;
        if (!listas.empty() && listas[0] != nullptr) {
            NodoDoc* base = listas[0];
            while (base) {
                bool comun = true;
                for (size_t j = 1; j < listas.size(); ++j) {
                    NodoDoc* aux = listas[j];
                    bool encontrado = false;
                    while (aux) {
                        if (aux->idDocumento == base->idDocumento) {
                            encontrado = true;
                            break;
                        }
                        aux = aux->siguiente;
                    }
                    if (!encontrado) {
                        comun = false;
                        break;
                    }
                }
                if (comun) resultado.insert(base->idDocumento);
                base = base->siguiente;
            }
        }

        if (!resultado.empty()) {
            consultasUsadas++;
            std::vector<int> topDocs(resultado.begin(), resultado.end());
            if (topDocs.size() > 10) topDocs.resize(10);
            for (size_t j = 0; j < topDocs.size(); ++j) {
                for (size_t k = j + 1; k < topDocs.size(); ++k) {
                    agregarArista(grafo, topDocs[j], topDocs[k]);
                }
            }
        }
        int progreso = (i * 100) / todasConsultas.size();
        std::cout << "\rProgreso grafo: " << progreso << "%" << std::flush;
    }

    auto endGrafo = std::chrono::steady_clock::now();
    std::cout << "\nGrafo de co-relevancia construido.\n";

    int maxIteraciones = 100;
    double d = 0.85;
    double tolerancia = 1e-6;

    auto startPR = std::chrono::steady_clock::now();
    int iteracionesReales = calcularPageRank(grafo, maxIteraciones, d, tolerancia);
    auto endPR = std::chrono::steady_clock::now();

    mostrarMétricas(grafo, consultasUsadas, iteracionesReales);

    auto durGrafo = std::chrono::duration<double>(endGrafo - startGrafo).count();
    auto durPR = std::chrono::duration<double>(endPR - startPR).count();

    std::cout << "Tiempo de construcción del grafo: " << durGrafo << " segundos\n";
    std::cout << "Tiempo de cálculo de PageRank: " << durPR << " segundos\n";

    // --- CAMBIO: Bucle para 5 consultas manuales ---
    std::cout << "\n--- FASE DE PRUEBA: CONSULTAS MANUALES ---\n";
    std::string consultaManual;
    for (int i = 0; i < 5; ++i) {
        std::cout << "\nIngrese consulta de prueba " << (i + 1) << "/5 (o escriba 'salir'): ";
        std::getline(std::cin, consultaManual);

        if (consultaManual == "salir") {
            break;
        }

        std::istringstream iss(consultaManual);
        std::string palabra;
        std::vector<NodoDoc*> listas;

        while (iss >> palabra) {
            std::string limpia = limpiarPalabra(palabra);
            if (!limpia.empty() && !esStopword(limpia, stopwords)) {
                NodoTermino* nodo = buscarTermino(indice, limpia);
                if (nodo) listas.push_back(nodo->listaDocumentos);
            }
        }

        std::set<int> resultado;
        if (!listas.empty() && listas[0] != nullptr) {
            NodoDoc* base = listas[0];
            while (base) {
                bool comun = true;
                for (size_t j = 1; j < listas.size(); ++j) {
                    NodoDoc* aux = listas[j];
                    bool encontrado = false;
                    while (aux) {
                        if (aux->idDocumento == base->idDocumento) {
                            encontrado = true;
                            break;
                        }
                        aux = aux->siguiente;
                    }
                    if (!encontrado) {
                        comun = false;
                        break;
                    }
                }
                if (comun) resultado.insert(base->idDocumento);
                base = base->siguiente;
            }
        }

        if (!resultado.empty()) {
            std::cout << "Resultados sin PageRank (por DocID): ";
            for (int id : resultado) std::cout << id << " ";
            std::cout << "\n";

            std::vector<std::pair<int, double>> ranking;
            for (int id : resultado) {
                double pr = grafo.nodos.count(id) ? grafo.nodos.at(id).pagerank : 0.0;
                ranking.push_back({id, pr});
            }
            std::sort(ranking.begin(), ranking.end(), [](auto& a, auto& b) {
                return b.second < a.second;
            });

            std::cout << "Resultados con PageRank (DocID | Score): ";
            for (const auto& [id, score] : ranking) {
                std::cout << "[" << id << " | " << std::fixed << std::setprecision(6) << score << "] ";
            }
            std::cout << "\n";
        } else {
            std::cout << "No se encontraron resultados para la consulta.\n";
        }
    }

    liberarIndice(indice);
    std::cout << "\nProceso completado.\n";
    return 0;
}
