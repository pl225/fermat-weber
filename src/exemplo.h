#ifndef EXEMPLO_H
#define EXEMPLO_H

#include "tipos.h"
#include <vector>

typedef struct instancia {
    std::vector<Coordenada> coordenadas;
    std::vector<Aresta> arestas;
} Instancia;

Instancia problemaUm();
Instancia problemaDois();

#endif