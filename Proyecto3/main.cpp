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

// funcion clave para la cache
// la idea es que "big house" y "house big" den la misma clave para que funcione el cache
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

    //ordenar las palabras alfabeticamente
    sort(palabras.begin(), palabras.end());

    // unir las palabras para crear la clave final
    string clave_normalizada;
    for (size_t i = 0; i < palabras.size(); ++i) {
        clave_normalizada += palabras[i] + (i == palabras.size() - 1 ? "" : " ");
    }
    
    return clave_normalizada;
}

// funcion para buscar el puntaje de pagerank de un doc especifico
double obtenerScorePageRank(const Grafo& grafo, int id) {
    // revisamos que el grafo no este vacio o corrupto
    if (!grafo.pageRank || !grafo.ids || grafo.numNodos == 0) {
        cerr << "❌ Error: grafo no inicializado correctamente.\n";
        return 0.0;
    }
    //buscamos el id en el array de ids y devolvemos el puntaje que esta en la misma posicion
    for (int i = 0; i < grafo.numNodos; ++i)
        if (grafo.ids[i] == id)
            return grafo.pageRank[i];
    return 0.0; // si no lo encuentra, score 0
}

int main(int argc, char* argv[]) {
    // revisamos que nos pasen los 3 archivos
    if (argc != 4) {
        cout << "Uso: " << argv[0] << " <documentos.dat> <consultas.dat> <stopwords.txt>\n";
        return 1;
    }

    string archivoDocumentos = argv[1];
    string archivoConsultas = argv[2];
    string archivoStopwords = argv[3];

    // Construir el Indice Invertido (Logica del P1) 
    IndiceInvertido indice;
    inicializarIndice(indice);
    set<string> stopwords;
    cargarStopwords(archivoStopwords, stopwords); //cargamos las stopwords para ignorarlas

    // leemos el archivo de documentos linea por linea
    ifstream documentos(archivoDocumentos);
    string linea;
    int idDoc = 1;
    while (getline(documentos, linea)) {
        size_t pos = linea.rfind("||");
        if (pos != string::npos) {
            string contenido = linea.substr(pos + 2);
            istringstream iss(contenido);
            string palabra;
            // y procesamos palabra por palabra
            while (iss >> palabra) {
                string limpia = limpiarPalabra(palabra);
                // si no es stopword, la metemos al indice
                if (!limpia.empty() && !esStopword(limpia, stopwords))
                    insertarTermino(indice, limpia, idDoc);
            }
        }
        idDoc++;
    }
    documentos.close();

    // construir el Grafo (Logica del P2 - Offline) 
    Grafo grafo;
    high_resolution_clock::time_point inicioGrafo = high_resolution_clock::now();
    inicializarGrafo(grafo, 2000); // 2000 es el maximo de nodos que esperamos

    // leemos todas las consultas del log para construir el grafo
    ifstream consultas(archivoConsultas);
    vector<string> todasConsultas;
    while (getline(consultas, linea)) {
        if (!linea.empty()) todasConsultas.push_back(linea);
    }
    consultas.close();

    // ahora procesamos cada consulta del log
    size_t total = todasConsultas.size();
    for (size_t i = 0; i < total; ++i) {
        const auto& consulta = todasConsultas[i];

        // esto es para mostrar el progreso en la consola
        double pct = (i + 1) * 100.0 / total;
        cout << fixed << setprecision(2)
             << "⏳ Construyendo grafo... " << (i + 1) << "/" << total
             << " (" << pct << "%)\r" << flush;
        
        // buscamos los resultados de la consulta en nuestro indice
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

        // esta es la logica de interseccion de listas
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

        // si la consulta dio resultados
        if (!resultado.empty()) {
            // tomamos solo los primeros 10 (top-K)
            vector<int> topDocs(resultado.begin(), resultado.end());
            if (topDocs.size() > 10) topDocs.resize(10);
            // y creamos una arista entre cada par de documentos
            for (size_t j = 0; j < topDocs.size(); ++j)
                for (size_t k = j + 1; k < topDocs.size(); ++k)
                    agregarArista(grafo, topDocs[j], topDocs[k]);
        }
    }
    cout << "\nGrafo construido." << endl;

    high_resolution_clock::time_point finGrafo = high_resolution_clock::now();
    duration<double> tiempoGrafo = duration_cast<duration<double>>(finGrafo - inicioGrafo);

    // Calcular PageRank 
    high_resolution_clock::time_point inicioPR = high_resolution_clock::now();
    int iteracionesPageRank = calcularPageRank(grafo, 20, 0.85); 
    high_resolution_clock::time_point finPR = high_resolution_clock::now();
    duration<double> tiempoPR = duration_cast<duration<double>>(finPR - inicioPR);

    // Mostramos un resumen de todo el proceso offline
    cout << "\n--- Métricas del Grafo (Fase Offline) ---\n";
    cout << "Consultas usadas para construir el grafo: " << todasConsultas.size() << "\n";
    cout << "Nodos en el grafo: " << grafo.numNodos << "\n";
    cout << "Aristas en el grafo: " << grafo.numAristas << "\n";
    cout << "PageRank convergió en: " << iteracionesPageRank << " iteraciones\n";
    cout << "Tiempo de construcción del grafo: " << fixed << setprecision(3) << tiempoGrafo.count() << " segundos\n";
    cout << "Tiempo de cálculo de PageRank: " << fixed << setprecision(3) << tiempoPR.count() << " segundos\n";
    
    // bucle Interactivo con Cache (Logica del P3) 
    Cache cache;
    inicializarCache(cache, 10, 53); // inicializamos nuestra cache con capacidad 10
    int hits = 0, misses = 0, reemplazos = 0, inserciones = 0; // variables para contar las metricas

    string consultaInput;
    cout << "\n✅ Sistema listo. Ingrese su consulta." << endl;

    // el bucle principal, se ejecuta hasta que el usuario escriba "exit"
    while (true) {
        cout << "\n> ";
        getline(cin, consultaInput);
        if (consultaInput == "exit") break;

        // normalizamos la consulta para usarla como clave en la cache
        string claveCache = normalizarConsulta(consultaInput);
        vector<int> resultados;

        // buscamos en la cache primero
        if (buscarCache(cache, claveCache, resultados)) {
            // si esta es un hit
            hits++;
            cout << "✅ HIT en caché\n";
        } else {
            // si no esta es un miss
            misses++;
            cout << "❌ MISS - buscando en índice\n";
            
            // asi que tenemos que buscar en el indice principal
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

            // usamos la misma logica de interseccion de antes
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
                                encontrado = true; break;
                            }
                            aux = aux->siguiente;
                        }
                        if (!encontrado) { comun = false; break; }
                    }
                    if (comun) resultadoSet.insert(base->idDocumento);
                    base = base->siguiente;
                }
            }

            // una vez que tenemos los resultados, los metemos a la cache
            resultados.assign(resultadoSet.begin(), resultadoSet.end());
            // si la cache ya estaba llena provoca un reemplazo
            if (cache.size >= cache.capacidad) reemplazos++;
            insertarCache(cache, claveCache, resultados);
            inserciones++;
        }

        // mostramos los resultados tal como vienen del indice
        cout << "Resultados sin PageRank: ";
        for (int id : resultados) cout << id << " ";
        cout << endl;

        // ahora buscamos el score de pagerank para cada resultado y lo ordenamos
        vector<pair<int, double>> ordenados;
        for (int id : resultados)
            ordenados.emplace_back(id, obtenerScorePageRank(grafo, id));

        sort(ordenados.begin(), ordenados.end(),
             [](auto& a, auto& b) { return a.second > b.second; });

        // y mostramos los resultados ordenados por importancia
        cout << "Resultados con PageRank: ";
        for (auto& par : ordenados)
            cout << "[" << par.first << " | " << fixed << setprecision(6) << par.second << "] ";
        cout << endl;

        // tambien mostramos como quedo la cache para ver el orden LRU
        mostrarEstadoCache(cache);
    }

    // Al salir del bucle, mostramos las metricas de la sesion
    int totalConsultas = hits + misses;
    double tasaAciertos = (totalConsultas == 0 ? 0 : (hits * 100.0 / totalConsultas));
    double tasaFallos   = (totalConsultas == 0 ? 0 : (misses * 100.0 / totalConsultas));

    cout << "\n--- Métricas Finales de la Sesión ---\n";
    cout << "Total consultas: " << totalConsultas << "\n";
    cout << "Hits: " << hits << "\n";
    cout << "Misses: " << misses << "\n";
    cout << "Tasa aciertos: " << fixed << setprecision(2) << tasaAciertos << "%\n";
    cout << "Tasa fallos: " << fixed << setprecision(2) << tasaFallos << "%\n";
    cout << "Reemplazos: " << reemplazos << "\n";
    cout << "Inserciones en caché: " << inserciones << "\n";
    cout << "Elementos actuales en caché: " << cache.size << "\n";

    // liberar toda la memoria que pedimos
    liberarCache(cache);
    liberarIndice(indice);
    liberarGrafo(grafo);
    
    return 0;
}
