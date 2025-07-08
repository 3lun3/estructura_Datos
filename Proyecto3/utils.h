#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <set>

// include guard para que no se incluya mil veces

// para pasar a minuscula
std::string aMinuscula(const std::string& palabra);

// para quitarle la basura a las palabras (puntos, comas, etc)
std::string limpiarPalabra(const std::string& palabra);

// para leer el archivo de stopwords
void cargarStopwords(const std::string& nombreArchivo, std::set<std::string>& stopwords);

// para ver si una palabra es stopword
bool esStopword(const std::string& palabra, const std::set<std::string>& stopwords);

#endif // UTILS_H
