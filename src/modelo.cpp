#include "modelo.h"

GRBModel criarModelo(
    GRBEnv &env, 
    std::vector<Coordenada> &coordenadas,
    std::vector<Aresta> &arestas
) {
    GRBModel model = GRBModel(env);

    return model;
}