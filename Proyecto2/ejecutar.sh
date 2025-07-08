#!/bin/bash

# Script para compilar y ejecutar el proyecto
if [ "$#" -ne 3 ]; then
    echo "Uso: $0 <archivo_documentos> <archivo_consultas> <archivo_stopwords>"
    exit 1
fi

# Compilar
echo "🔧 Compilando el proyecto..."
g++ -o buscador main.cpp index.cpp grafo.cpp utils.cpp

if [ $? -ne 0 ]; then
    echo "❌ Error de compilación. Abortando."
    exit 1
fi

# Ejecutar
echo "🚀 Ejecutando..."
./buscador "$1" "$2" "$3"