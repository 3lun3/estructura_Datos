#!/bin/bash
echo "🔧 Compilando proyecto "

g++ -o buscador main.cpp index.cpp utils.cpp grafo.cpp cache.cpp

if [ $? -eq 0 ]; then
    echo "✅ Compilación exitosa."
else
    echo "❌ Error en la compilación."
fi

