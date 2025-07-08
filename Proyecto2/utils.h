#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <set>

// Convierte una palabra a minúsculas
std::string aMinuscula(const std::string& palabra);

// Limpia una palabra eliminando signos y dejando solo letras
std::string limpiarPalabra(const std::string& palabra);

// Carga las stopwords desde un archivo en un set
void cargarStopwords(const std::string& nombreArchivo, std::set<std::string>& stopwords);

// Verifica si una palabra está en el set de stopwords
bool esStopword(const std::string& palabra, const std::set<std::string>& stopwords);

#endif
