#include "modelo.h"
#include <math.h>

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

int calcularM(
    std::vector<Coordenada> &coordenadas
) {
    double maior = 0;
    for (size_t i = 0; i < coordenadas.size(); i++) {
        for (size_t j = i + 1; j < coordenadas.size(); j++) {
            double distancia = sqrt(
                pow(coordenadas[j].first - coordenadas[i].first, 2)
                + pow(coordenadas[j].second - coordenadas[i].second, 2)
            );

            if (distancia > maior) {
                maior = distancia;
            }
        }
    }

    return (int) ceil(maior);
}

GRBModel criarModeloMaculan(
    GRBEnv &env, 
    int numT,
    std::vector<Coordenada> &coordenadas
) {
    GRBModel model = GRBModel(env);
    int numS = numT - 2;
    const int M = calcularM(coordenadas);

    std::vector<std::vector<GRBVar>> y, z;
    std::vector<std::vector<std::vector<GRBVar>>> t;

    for (int i = 0; i < numS; i++) {
        y.push_back(std::vector<GRBVar>());
        z.push_back(std::vector<GRBVar>());
        t.push_back(std::vector<std::vector<GRBVar>>());
        
        for (int j = 0; j < numT; j++) {
            std::string indexJ = std::to_string(j + 1), 
                indexI = std::to_string(numT + i + 1);
            
            y[i].push_back(model.addVar(0, 1, 0, GRB_BINARY, "y_" + indexJ + ',' + indexI));
            z[i].push_back(model.addVar(0, GRB_INFINITY, 1, GRB_CONTINUOUS, "z_" + indexJ + ',' + indexI));

            t[i].push_back(std::vector<GRBVar>());
            for (int k = 0; k < DIMENSAO; k++) {
                t[i][j].push_back(model.addVar(-GRB_INFINITY, GRB_INFINITY, 0, GRB_CONTINUOUS, "t_" + indexJ + ',' + indexI + ',' + std::to_string(k + 1)));
            }
        }

        for (int j = i + 1; j < numS; j++) {
            std::string indexJ = std::to_string(numT + j + 1), 
                indexI = std::to_string(numT + i + 1);
                
            y[i].push_back(model.addVar(0, 1, 0, GRB_BINARY, "y_" + indexI + ',' + indexJ));
            z[i].push_back(model.addVar(0, GRB_INFINITY, 1, GRB_CONTINUOUS, "z_" + indexI + ',' + indexJ));

            t[i].push_back(std::vector<GRBVar>());
            size_t index = t[i].size() - 1;
            for (int k = 0; k < DIMENSAO; k++) {
                t[i][index].push_back(model.addVar(-GRB_INFINITY, GRB_INFINITY, 0, GRB_CONTINUOUS, "t_" + indexI + ',' + indexJ + ',' + std::to_string(k + 1)));
            }
        }
    }
    
    return model;
}