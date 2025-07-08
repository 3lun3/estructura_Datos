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
    // Verificamos que se hayan pasado los 3 archivos necesarios como argumentos
    if (argc != 4) {
        std::cout << "Uso: " << argv[0] << " <archivo_documentos> <archivo_consultas> <archivo_stopwords>\n";
        return 1; // Si no es así, mostramos un mensaje de uso y terminamos el programa
    }

    // Asignamos los archivos a variables
    std::string archivoDocumentos = argv[1]; // Archivo de documentos
    std::string archivoConsultas = argv[2];  // Archivo de consultas
    std::string archivoStopwords = argv[3];  // Archivo de stopwords

    // Inicializamos el índice invertido
    IndiceInvertido indice;
    inicializarIndice(indice);

    // Cargamos las stopwords desde el archivo de stopwords
    std::set<std::string> stopwords; // Usamos un set para que no haya palabras duplicadas
    cargarStopwords(archivoStopwords, stopwords);

    // Construimos el índice invertido a partir de los documentos
    std::ifstream documentos(archivoDocumentos); // Abrimos el archivo de documentos
    std::string linea;
    int idDoc = 1; // Iniciamos el ID del documento

    // Primero contamos cuántos documentos hay en total para mostrar el progreso
    int totalDocs = 0;
    std::ifstream contar(archivoDocumentos); // Abrimos el archivo nuevamente para contar las líneas
    while (std::getline(contar, linea)) totalDocs++; // Contamos cuántas líneas hay
    contar.close(); // Cerramos el archivo

    // Mostramos un mensaje de inicio para la construcción del índice
    std::cout << "Construyendo índice invertido...\n";
    while (std::getline(documentos, linea)) {
        size_t pos = linea.rfind("||"); // Buscamos la última aparición de "||" para separar el contenido
        if (pos != std::string::npos) {
            std::string contenido = linea.substr(pos + 2); // Extraemos el contenido después de "||"
            std::istringstream iss(contenido); // Creamos un flujo para leer el contenido palabra por palabra
            std::string palabra;
            while (iss >> palabra) { // Leemos cada palabra
                // Limpiamos la palabra y la convertimos a minúsculas
                std::string limpia = limpiarPalabra(palabra);
                // Si la palabra no está vacía y no es una stopword, la insertamos en el índice
                if (!limpia.empty() && !esStopword(limpia, stopwords)) {
                    insertarTermino(indice, limpia, idDoc); // Insertamos la palabra en el índice invertido
                }
            }
        }

        // Calculamos el progreso de la construcción del índice y lo mostramos
        int progreso = (idDoc * 100) / totalDocs;
        std::cout << "\rProgreso índice: " << progreso << "%" << std::flush;
        idDoc++; // Incrementamos el ID del documento
    }
    documentos.close(); // Cerramos el archivo de documentos
    std::cout << "\nÍndice invertido construido.\n";

    // Construimos el grafo de co-relevancia a partir de las consultas
    Grafo grafo; // Creamos el grafo vacío
    std::ifstream consultas(archivoConsultas); // Abrimos el archivo de consultas
    std::vector<std::string> todasConsultas; // Usamos un vector para almacenar todas las consultas
    while (std::getline(consultas, linea)) {
        if (!linea.empty()) todasConsultas.push_back(linea); // Añadimos cada consulta al vector
    }
    consultas.close(); // Cerramos el archivo de consultas

    int consultasUsadas = 0; // Contador de las consultas que efectivamente generaron resultados
    auto startGrafo = std::chrono::steady_clock::now(); // Iniciamos la medición del tiempo para la construcción del grafo

    std::cout << "Construyendo grafo de co-relevancia...\n";
    for (size_t i = 0; i < todasConsultas.size(); ++i) { // Recorremos todas las consultas
        std::string consulta = todasConsultas[i]; // Tomamos la consulta actual
        std::istringstream iss(consulta); // Creamos un flujo de entrada con la consulta
        std::string palabra;
        std::vector<NodoDoc*> listas; // Lista de documentos relevantes para la consulta

        while (iss >> palabra) { // Procesamos cada palabra de la consulta
            std::string limpia = limpiarPalabra(palabra); // Limpiamos la palabra
            if (!limpia.empty() && !esStopword(limpia, stopwords)) { // Si la palabra es válida
                NodoTermino* nodo = buscarTermino(indice, limpia); // Buscamos el término en el índice
                if (nodo) listas.push_back(nodo->listaDocumentos); // Si el término está en el índice, lo agregamos a la lista
            }
        }

        std::set<int> resultado; // Aquí guardamos los documentos que son relevantes para la consulta
        if (!listas.empty() && listas[0] != nullptr) { // Si la lista no está vacía
            NodoDoc* base = listas[0]; // Empezamos a comparar los documentos
            while (base) { // Comprobamos si el documento está en todos los conjuntos
                bool comun = true;
                for (size_t j = 1; j < listas.size(); ++j) { // Recorremos los demás documentos
                    NodoDoc* aux = listas[j];
                    bool encontrado = false;
                    while (aux) { // Buscamos si el documento está presente
                        if (aux->idDocumento == base->idDocumento) { // Si lo encontramos
                            encontrado = true;
                            break;
                        }
                        aux = aux->siguiente; // Avanzamos al siguiente documento
                    }
                    if (!encontrado) { // Si no está en alguno, lo marcamos como no común
                        comun = false;
                        break;
                    }
                }
                if (comun) resultado.insert(base->idDocumento); // Si el documento está en todos, lo agregamos
                base = base->siguiente; // Pasamos al siguiente documento
            }
        }

        if (!resultado.empty()) { // Si encontramos documentos relevantes
            consultasUsadas++; // Aumentamos el contador de consultas usadas
            std::vector<int> topDocs(resultado.begin(), resultado.end()); // Convertimos el set a un vector
            if (topDocs.size() > 10) topDocs.resize(10); // Limitamos a los 10 primeros documentos

            // Añadimos las aristas entre los documentos relevantes en el grafo
            for (size_t j = 0; j < topDocs.size(); ++j) {
                for (size_t k = j + 1; k < topDocs.size(); ++k) {
                    agregarArista(grafo, topDocs[j], topDocs[k]);
                }
            }
        }

        // Mostramos el progreso de la construcción del grafo
        int progreso = (i * 100) / todasConsultas.size();
        std::cout << "\rProgreso grafo: " << progreso << "%" << std::flush;
    }

    auto endGrafo = std::chrono::steady_clock::now(); // Medimos el tiempo final para el grafo
    std::cout << "\nGrafo de co-relevancia construido.\n";

    // Parámetros de PageRank
    int maxIteraciones = 100; // Número máximo de iteraciones
    double d = 0.85; // Factor de amortiguación (generalmente se usa 0.85)
    double tolerancia = 1e-6; // Tolerancia para considerar que PageRank ha convergido

    auto startPR = std::chrono::steady_clock::now(); // Empezamos el cálculo de PageRank
    int iteracionesReales = calcularPageRank(grafo, maxIteraciones, d, tolerancia); // Calculamos PageRank
    auto endPR = std::chrono::steady_clock::now(); // Medimos el tiempo final del cálculo de PageRank

    mostrarMétricas(grafo, consultasUsadas, iteracionesReales); // Mostramos las métricas del grafo

    // Calculamos los tiempos totales
    auto durGrafo = std::chrono::duration<double>(endGrafo - startGrafo).count();
    auto durPR = std::chrono::duration<double>(endPR - startPR).count();

    std::cout << "Tiempo de construcción del grafo: " << durGrafo << " segundos\n";
    std::cout << "Tiempo de cálculo de PageRank: " << durPR << " segundos\n";

    
    std::cout << "\n--- FASE DE PRUEBA: CONSULTAS MANUALES ---\n";
    std::string consultaManual;
    for (int i = 0; i < 5; ++i) {
        std::cout << "\nIngrese consulta de prueba " << (i + 1) << "/5 (o escriba 'salir'): ";
        std::getline(std::cin, consultaManual); // Leemos la consulta del usuario

        if (consultaManual == "salir") {
            break; // Si el usuario escribe "salir", terminamos el bucle
        }

        std::istringstream iss(consultaManual); // Creamos un flujo para procesar la consulta
        std::string palabra;
        std::vector<NodoDoc*> listas; // Lista de documentos relevantes para la consulta

        while (iss >> palabra) { // Procesamos cada palabra de la consulta
            std::string limpia = limpiarPalabra(palabra);
            if (!limpia.empty() && !esStopword(limpia, stopwords)) { // Si es válida
                NodoTermino* nodo = buscarTermino(indice, limpia);
                if (nodo) listas.push_back(nodo->listaDocumentos);
            }
        }

     std::set<int> resultado; // Usamos un set para almacenar los documentos que coinciden con todos los términos de la consulta
if (!listas.empty() && listas[0] != nullptr) {  // Si la lista de documentos no está vacía y no es nula
    NodoDoc* base = listas[0]; // Tomamos el primer conjunto de documentos (de la primera palabra de la consulta)
    
    while (base) {  // Recorremos la lista de documentos del primer término
        bool comun = true; // Inicializamos una bandera que verificará si este documento está en todos los conjuntos

        for (size_t j = 1; j < listas.size(); ++j) { // Iteramos sobre los otros conjuntos de documentos correspondientes a los demás términos
            NodoDoc* aux = listas[j];  // Empezamos con la lista de documentos de otro término
            bool encontrado = false;  // Inicializamos una bandera para saber si encontramos el documento en esta lista

            while (aux) {  // Recorremos la lista de documentos del término actual
                if (aux->idDocumento == base->idDocumento) {  // Si encontramos el mismo documento que en la primera lista
                    encontrado = true;  // Marcamos que encontramos el documento
                    break;  // Terminamos la búsqueda en esta lista, ya que lo encontramos
                }
                aux = aux->siguiente;  // Avanzamos al siguiente documento
            }

            if (!encontrado) {  // Si no encontramos el documento en alguna lista, significa que no es común a todos los términos
                comun = false;  // Marcamos que este documento no es común y salimos del bucle
                break;  // No seguimos buscando más, ya que este documento no es relevante para la consulta
            }
        }

        if (comun) {  // Si el documento está en todas las listas (es decir, es relevante para todos los términos)
            resultado.insert(base->idDocumento);  // Lo agregamos al conjunto de resultados
        }

        base = base->siguiente;  // Pasamos al siguiente documento de la lista de la primera palabra
    }
}

            }
        }

        if (!resultado.empty()) { // Si encontramos resultados
            std::cout << "Resultados sin PageRank (por DocID): ";
            for (int id : resultado) std::cout << id << " ";
            std::cout << "\n";

            std::vector<std::pair<int, double>> ranking;
            for (int id : resultado) {
                double pr = grafo.nodos.count(id) ? grafo.nodos.at(id).pagerank : 0.0;
                ranking.push_back({id, pr}); // Creamos el ranking con los PageRank
            }
            std::sort(ranking.begin(), ranking.end(), [](auto& a, auto& b) {
                return b.second < a.second; // Ordenamos por PageRank de mayor a menor
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

    liberarIndice(indice); // Liberamos la memoria del índice
    std::cout << "\nProceso completado.\n";
    return 0;
}

