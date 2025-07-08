#include "utils.h"
#include <fstream>
#include <cctype>
#include <algorithm>

// Convierte una palabra a minúsculas
std::string aMinuscula(const std::string& palabra) {
    std::string resultado = palabra;
    std::transform(resultado.begin(), resultado.end(), resultado.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return resultado;
}

// Limpia una palabra eliminando caracteres no alfabéticos
std::string limpiarPalabra(const std::string& palabra) {
    std::string resultado;
    for (char c : palabra) {
        if (std::isalpha(static_cast<unsigned char>(c))) {
            resultado += std::tolower(c);
        }
    }
    return resultado;
}

// Carga las stopwords desde un archivo
void cargarStopwords(const std::string& nombreArchivo, std::set<std::string>& stopwords) {
    std::ifstream archivo(nombreArchivo);
    std::string palabra;
    while (archivo >> palabra) {
        stopwords.insert(aMinuscula(palabra));
    }
}

// Verifica si una palabra es una stopword
bool esStopword(const std::string& palabra, const std::set<std::string>& stopwords) {
    return stopwords.find(palabra) != stopwords.end();
}
