#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <algorithm>
#include <list>
#include <chrono>
#include <iomanip>
#include <cctype>

#include "index.h"
#include "utils.h"
#include "grafo.h"
#include "cache.h"

using namespace std;
using namespace std::chrono;


string normalizarConsulta(string s) {
    // poner en minusculas
    transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return tolower(c); });

    // separar las palabras en un vector
    istringstream iss(s);
    string palabra;
    vector<string> palabras;
    while (iss >> palabra) {
        palabras.push_back(palabra);
    }

    //ordenar las palabras alfabaticamente
    sort(palabras.begin(), palabras.end());

    // unir las palabras para crear la clave (big house = house big)
    string clave_normalizada;
    for (size_t i = 0; i < palabras.size(); ++i) {
        clave_normalizada += palabras[i] + (i == palabras.size() - 1 ? "" : " ");
    }
    
    return clave_normalizada;
}

double obtenerScorePageRank(const Grafo& grafo, int id) {
    if (!grafo.pageRank || !grafo.ids || grafo.numNodos == 0) {
        cerr << "❌ Error: grafo no inicializado correctamente.\n";
        return 0.0;
    }
    for (int i = 0; i < grafo.numNodos; ++i)
        if (grafo.ids[i] == id)
            return grafo.pageRank[i];
    return 0.0;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cout << "Uso: " << argv[0] << " <documentos.dat> <consultas.dat> <stopwords.txt>\n";
        return 1;
    }

    string archivoDocumentos = argv[1];
    string archivoConsultas = argv[2];
    string archivoStopwords = argv[3];

    IndiceInvertido indice;
    inicializarIndice(indice);
    set<string> stopwords;
    cargarStopwords(archivoStopwords, stopwords);

    ifstream documentos(archivoDocumentos);
    string linea;
    int idDoc = 1;
    while (getline(documentos, linea)) {
        size_t pos = linea.rfind("||");
        if (pos != string::npos) {
            string contenido = linea.substr(pos + 2);
            istringstream iss(contenido);
            string palabra;
            while (iss >> palabra) {
                string limpia = limpiarPalabra(palabra);
                if (!limpia.empty() && !esStopword(limpia, stopwords))
                    insertarTermino(indice, limpia, idDoc);
            }
        }
        idDoc++;
    }
    documentos.close();

    Grafo grafo;
    high_resolution_clock::time_point inicioGrafo = high_resolution_clock::now();
    inicializarGrafo(grafo, 2000);

    ifstream consultas(archivoConsultas);
    vector<string> todasConsultas;
    while (getline(consultas, linea)) {
        if (!linea.empty()) todasConsultas.push_back(linea);
    }
    consultas.close();

    size_t total = todasConsultas.size();
    for (size_t i = 0; i < total; ++i) {
        const auto& consulta = todasConsultas[i];

        // Mostrar progreso
        double pct = (i + 1) * 100.0 / total;
        cout << fixed << setprecision(2)
             << "⏳ Procesando consulta " << (i + 1) << "/" << total
             << " (" << pct << "% completado)\r" << flush;
        

        istringstream iss(consulta);
        string palabra;
        vector<NodoDoc*> listas;
        while (iss >> palabra) {
            string limpia = limpiarPalabra(palabra);
            if (!limpia.empty() && !esStopword(limpia, stopwords)) {
                NodoTermino* nodo = buscarTermino(indice, limpia);
                if (nodo) listas.push_back(nodo->listaDocumentos);
            }
        }

        set<int> resultado;
        if (!listas.empty() && listas[0]) {
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
                    if (!encontrado) { comun = false; break; }
                }
                if (comun) resultado.insert(base->idDocumento);
                base = base->siguiente;
            }
        }

        if (!resultado.empty()) {
            vector<int> topDocs(resultado.begin(), resultado.end());
            if (topDocs.size() > 10) topDocs.resize(10);
            for (size_t j = 0; j < topDocs.size(); ++j)
                for (size_t k = j + 1; k < topDocs.size(); ++k)
                    agregarArista(grafo, topDocs[j], topDocs[k]);
        }
    }
    cout << endl;

    high_resolution_clock::time_point finGrafo = high_resolution_clock::now();
    duration<double> tiempoGrafo = duration_cast<duration<double>>(finGrafo - inicioGrafo);

    high_resolution_clock::time_point inicioPR = high_resolution_clock::now();
    int iteracionesPageRank = calcularPageRank(grafo, 20, 0.85); 
    high_resolution_clock::time_point finPR = high_resolution_clock::now();
    duration<double> tiempoPR = duration_cast<duration<double>>(finPR - inicioPR);

    cout << "\n--- Métricas del Grafo (Proyecto 3) ---\n";
    cout << "Consultas usadas para construir el grafo: " << todasConsultas.size() << "\n";
    cout << "Nodos en el grafo: " << grafo.numNodos << "\n";
    cout << "Aristas en el grafo: " << grafo.numAristas << "\n";
    //cout << "PageRank convergió en: " << iteracionesPageRank << " iteraciones\n";
    cout << "Tiempo de construcción del grafo: " << fixed << setprecision(3) << tiempoGrafo.count() << " segundos\n";
    //cout << "Tiempo de cálculo de PageRank: " << fixed << setprecision(3) << tiempoPR.count() << " segundos\n";
    
    Cache cache;
    inicializarCache(cache, 10, 53);
    int hits = 0, misses = 0, reemplazos = 0, inserciones = 0;

    string consultaInput;
    while (true) {

        cout << "\nIngrese consulta (exit para salir): ";
        getline(cin, consultaInput);
        if (consultaInput == "exit") break;

        consultaInput = normalizarConsulta(consultaInput);
        vector<int> resultados;

        if (buscarCache(cache, consultaInput, resultados)) {
            hits++;
            cout << "✅ HIT en caché\n";
        } else {
            misses++;
            cout << "❌ MISS - buscando en índice\n";

            istringstream iss(consultaInput);
            string palabra;
            vector<NodoDoc*> listas;
            while (iss >> palabra) {
                string limpia = limpiarPalabra(palabra);
                if (!limpia.empty() && !esStopword(limpia, stopwords)) {
                    NodoTermino* nodo = buscarTermino(indice, limpia);
                    if (nodo) listas.push_back(nodo->listaDocumentos);
                }
            }

            set<int> resultadoSet;
            if (!listas.empty() && listas[0]) {
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
                        if (!encontrado) { comun = false; break; }
                    }
                    if (comun) resultadoSet.insert(base->idDocumento);
                    base = base->siguiente;
                }
            }

            resultados.assign(resultadoSet.begin(), resultadoSet.end());
            if (cache.size >= cache.capacidad) reemplazos++;
            insertarCache(cache, consultaInput, resultados);
            inserciones++;
        }

        cout << "Resultados sin PageRank: ";
        for (int id : resultados) cout << id << " ";
        cout << endl;

        vector<pair<int, double>> ordenados;
        for (int id : resultados)
            ordenados.emplace_back(id, obtenerScorePageRank(grafo, id));

        sort(ordenados.begin(), ordenados.end(),
             [](auto& a, auto& b) { return a.second > b.second; });

        cout << "Resultados con PageRank: ";
        for (auto& par : ordenados)
            cout << "[" << par.first << " | " << fixed << setprecision(6) << par.second << "] ";
        cout << endl;

        mostrarEstadoCache(cache);
    }

    // Métricas finales
    int totalConsultas = hits + misses;
    double tasaAciertos = (totalConsultas == 0 ? 0 : (hits * 100.0 / totalConsultas));
    double tasaFallos   = (totalConsultas == 0 ? 0 : (misses * 100.0 / totalConsultas));

    

    cout << "\n--- Métricas finales ---\n";
    cout << "Total consultas: " << totalConsultas << "\n";
    cout << "Hits: " << hits << "\n";
    cout << "Misses: " << misses << "\n";
    cout << "Tasa aciertos: " << fixed << setprecision(2) << tasaAciertos << "%\n";
    cout << "Tasa fallos: " << fixed << setprecision(2) << tasaFallos << "%\n";
    cout << "Reemplazos: " << reemplazos << "\n";
    cout << "Inserciones en caché: " << inserciones << "\n";
    cout << "Elementos actuales en caché: " << cache.size << "\n";

    liberarCache(cache);
    liberarIndice(indice);
    liberarGrafo(grafo);
    return 0;
}









