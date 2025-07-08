#include "utils.h"
#include <fstream>  // Para leer archivos
#include <cctype>   // Para verificar caracteres alfabéticos y convertir a minúsculas
#include <algorithm> // Para usar std::transform

// Convierte una palabra a minúsculas
// Esta función toma una palabra y la convierte completamente a minúsculas
std::string aMinuscula(const std::string& palabra) {
    std::string resultado = palabra; // Copiamos la palabra original a 'resultado'
    std::transform(resultado.begin(), resultado.end(), resultado.begin(),
                   [](unsigned char c) { return std::tolower(c); });  // Usamos std::transform para convertir cada carácter a minúscula
    return resultado;  // Retornamos la palabra ya en minúsculas
}

// Esta función elimina cualquier carácter que no sea una letra, dejando solo letras alfabéticas
std::string limpiarPalabra(const std::string& palabra) {
    std::string resultado;  // Aquí vamos a guardar la palabra limpia
    for (char c : palabra) {  // Recorremos cada carácter de la palabra
        if (std::isalpha(static_cast<unsigned char>(c))) {  // Verificamos si el carácter es alfabético (letra)
            resultado += std::tolower(c);  // Si es alfabético, lo agregamos a 'resultado' en minúscula
        }
    }
    return resultado;  // Retornamos la palabra limpia
}

// Esta función abre un archivo y carga las stopwords en un set para evitar duplicados
void cargarStopwords(const std::string& nombreArchivo, std::set<std::string>& stopwords) {
    std::ifstream archivo(nombreArchivo);  // Abrimos el archivo de stopwords
    std::string palabra;  // Variable para almacenar cada palabra del archivo
    while (archivo >> palabra) {  // Leemos cada palabra del archivo
        stopwords.insert(aMinuscula(palabra));  // Convertimos la palabra a minúsculas y la insertamos en el set
    }
}

// Esta función comprueba si la palabra está en el set de stopwords
bool esStopword(const std::string& palabra, const std::set<std::string>& stopwords) {
    return stopwords.find(palabra) != stopwords.end();  // Si la palabra está en el set, retornamos 'true', sino 'false'
}

