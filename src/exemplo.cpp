#include "exemplo.h"

std::vector<Coordenada> construirCoordenadas() {
    std::vector<Coordenada> coordenadas;
    
    coordenadas.push_back({0, 1});
    coordenadas.push_back({5, 0});
    coordenadas.push_back({11, 1});
    coordenadas.push_back({4, 4});

    return coordenadas;
}

Instancia problemaUm() {
    std::vector<Coordenada> coordenadas = construirCoordenadas();

    std::vector<Aresta> arestas;

    arestas.push_back({4, 0});
    arestas.push_back({4, 3});
    arestas.push_back({4, 5});
    arestas.push_back({5, 1});
    arestas.push_back({5, 2});

    return { coordenadas, arestas };
}

Instancia problemaDois() {
    std::vector<Coordenada> coordenadas = construirCoordenadas();

    std::vector<Aresta> arestas;

    arestas.push_back({4, 0});
    arestas.push_back({4, 1});
    arestas.push_back({4, 5});
    arestas.push_back({5, 2});
    arestas.push_back({5, 3});

    return { coordenadas, arestas };
}