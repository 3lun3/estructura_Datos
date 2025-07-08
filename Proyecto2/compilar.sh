#!/bin/bash

# Script para compilar el proyecto
echo "🔧 Compilando el proyecto..."

g++ -o buscador main.cpp index.cpp grafo.cpp utils.cpp

if [ $? -eq 0 ]; then
    echo "✅ Compilación exitosa."
else
    echo "❌ Error al compilar."
    exit 1
fi
