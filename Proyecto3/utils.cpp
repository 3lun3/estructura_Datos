#include "utils.h"
#include <fstream>
#include <cctype>
#include <algorithm>

// una funcion simple para pasar un string a minusculas
std::string aMinuscula(const std::string& palabra) {
    std::string resultado = palabra;
    // usamos transform que es de la libreria algorithm
    std::transform(resultado.begin(), resultado.end(), resultado.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return resultado;
}

// funcion para limpiar las palabras
// le quita los signos de puntuacion y numeros, y la pasa a minuscula
std::string limpiarPalabra(const std::string& palabra) {
    std::string resultado;
    // recorremos la palabra caracter por caracter
    for (char c : palabra) {
        // si es una letra, la agregamos al resultado (ya en minuscula)
        if (std::isalpha(static_cast<unsigned char>(c))) {
            resultado += std::tolower(c);
        }
    }
    return resultado;
}

// funcion para leer el archivo de stopwords y meterlas en un set
void cargarStopwords(const std::string& nombreArchivo, std::set<std::string>& stopwords) {
    //abrimos el archivo
    std::ifstream archivo(nombreArchivo);
    std::string palabra;
    // leemos palabra por palabra hasta que se acabe el archivo
    while (archivo >> palabra) {
        // y la metemos al set (en minuscula para que siempre coincida)
        stopwords.insert(aMinuscula(palabra));
    }
}

// para chequear si una palabra es stopword o no
bool esStopword(const std::string& palabra, const std::set<std::string>& stopwords) {
    // usamos find() del set, que es super rapido. si no lo encuentra, devuelve stopwords.end()
    return stopwords.find(palabra) != stopwords.end();
}
