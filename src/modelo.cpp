#include "modelo.h"

#define DIMENSAO 2

GRBModel criarModelo(
    GRBEnv &env, 
    std::vector<Coordenada> &coordenadas,
    std::vector<Aresta> &arestas
) {
    GRBModel model = GRBModel(env);
    model.set(GRB_IntParam_NonConvex, 2);

    std::vector<GRBVar> d;
    for (size_t i = 0; i < arestas.size(); i++) {
        d.push_back(model.addVar(0, GRB_INFINITY, 1, GRB_CONTINUOUS, 'd' + std::to_string(i)));
    }

    GRBVar x[DIMENSAO][DIMENSAO];

    x[0][0] = model.addVar(-GRB_INFINITY, GRB_INFINITY, 0, GRB_CONTINUOUS, "x_{5,1}");
    x[0][1] = model.addVar(-GRB_INFINITY, GRB_INFINITY, 0, GRB_CONTINUOUS, "x_{5,2}");
    x[1][0] = model.addVar(-GRB_INFINITY, GRB_INFINITY, 0, GRB_CONTINUOUS, "x_{6,1}");
    x[1][1] = model.addVar(-GRB_INFINITY, GRB_INFINITY, 0, GRB_CONTINUOUS, "x_{6,2}");

    for (size_t i = 0; i < arestas.size() - 1; i++) {
        GRBQuadExpr expr = 0;

        Aresta aresta = arestas[i];
        int coordenada_ind = aresta.second;
        int x_ind = aresta.first < arestas.size() ? 0 : 1;

        double coordenada[DIMENSAO] = { 
            coordenadas[coordenada_ind].first, 
            coordenadas[coordenada_ind].second 
        };

        for (int i = 0; i < DIMENSAO; i++) {
            expr += (x[x_ind][i] - coordenada[i]) * (x[x_ind][i] - coordenada[i]);
        }

        model.addQConstr(d[i] * d[i] >= expr, 'd' + std::to_string(aresta.first) + '_' + std::to_string(aresta.second));
    }

    GRBQuadExpr expr = (x[0][0] - x[1][0]) * (x[0][0] - x[1][0]);
    expr += (x[0][1] - x[1][1]) * (x[0][1] - x[1][1]);

    model.addQConstr(d[4] * d[4] >= expr, "d_{4,5}");

    return model;
}