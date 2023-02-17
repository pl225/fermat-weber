#ifndef MODELO_H
#define MODELO_H

#include "gurobi_c++.h"
#include "tipos.h"

GRBModel criarModelo(
    GRBEnv &env, 
    std::vector<Coordenada> &coordenadas,
    std::vector<Aresta> &arestas
);

GRBModel criarModeloMaculan(
    GRBEnv &env,
    int numT,
    std::vector<Coordenada> &coordenadas  
);

#endif