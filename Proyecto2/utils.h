#ifndef UTILS_H
#define UTILS_H

#include <string>  
#include <set>     

// Esta función toma una palabra y devuelve una nueva palabra con todos los caracteres en minúsculas.
std::string aMinuscula(const std::string& palabra);

// Elimina caracteres no alfabéticos (como números y puntuación) y convierte todo a minúsculas.
std::string limpiarPalabra(const std::string& palabra);


// Lee un archivo que contiene stopwords y las almacena en un conjunto para que no haya duplicados
// Este conjunto de stopwords se usará para filtrar las palabras irrelevantes en las consultas.
void cargarStopwords(const std::string& nombreArchivo, std::set<std::string>& stopwords);

// Devuelve true si la palabra es una stopword, y false si no lo es.
bool esStopword(const std::string& palabra, const std::set<std::string>& stopwords);

#endif

