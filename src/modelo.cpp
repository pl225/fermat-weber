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

void criarVariaveisModeloMaculan(
    int numS,
    int numT,
    GRBModel &model,
    std::vector<std::vector<GRBVar>> &y, 
    std::vector<std::vector<GRBVar>> &z, 
    std::vector<std::vector<GRBVar>> &x,
    std::vector<std::vector<std::vector<GRBVar>>> &t
) {
    for (int i = 0; i < numS; i++) {
        y.push_back(std::vector<GRBVar>());
        z.push_back(std::vector<GRBVar>());
        x.push_back(std::vector<GRBVar>());
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

        for (int k = 0; k < DIMENSAO; k++) {
            x[i].push_back(model.addVar(-GRB_INFINITY, GRB_INFINITY, 0, GRB_CONTINUOUS, "x_{" + std::to_string(numT + i + 1) + ',' + std::to_string(k + 1) + "}"));
        }
    }
}

GRBLinExpr getExprMenorY(
    int indS,
    int numT,
    std::vector<std::vector<GRBVar>> &y
) {
    GRBLinExpr expr = 0;
    int numVariaveis = indS + 1, qtdVariaveis = 0;
    int linha = 0, coluna = numT + indS;        

    while (qtdVariaveis < numVariaveis) {
        expr += y[linha][coluna];
        coluna--;
        linha++;
        qtdVariaveis++;
    }

    return expr;
}

void criarRestricoesYModeloMaculan(
    int numS,
    int numT,
    GRBModel &model,
    std::vector<std::vector<GRBVar>> &y
) {
    for (int j = 0; j < numT; j++) {
        GRBLinExpr expr = 0;
        for (int i = 0; i < numS; i++) {
            expr += y[i][j];
        }
        model.addConstr(expr == 1);
    }

    for (int indS = 0; indS < numS - 1; indS++) {
        GRBLinExpr expr = getExprMenorY(indS, numT, y);
        model.addConstr(expr == 1);        
    }

    for (int indS = 0; indS < numS; indS++) {
        GRBLinExpr expr = 0;
        for (int j = 0; j < numT; j++) {
            expr += y[indS][j];
        }

        if (indS > 0) {
            expr += getExprMenorY(indS - 1, numT, y);
        }

        for (int j = numT; j < y[indS].size(); j++) {
            expr += y[indS][j];
        }
        model.addConstr(expr == 3);
    }
}

void criarRestricoesTModeloMaculan(
    int numS,
    int numT,
    GRBModel &model,
    int M,
    std::vector<Coordenada> &coordenadas,
    std::vector<std::vector<GRBVar>> &y, 
    std::vector<std::vector<GRBVar>> &x,
    std::vector<std::vector<std::vector<GRBVar>>> &t
) {
    for (int i = 0; i < numS; i++) {
        for (int j = 0; j < numT; j++) {
            for (int k = 0; k < DIMENSAO; k++) {
                model.addConstr((-1) * M * y[i][j] <= t[i][j][k]);
                model.addConstr(t[i][j][k] <= M * y[i][j]);

                double coordenada = k == 0 
                    ? coordenadas[j].first 
                    : coordenadas[j].second;

                model.addConstr((-1) * M * (1 - y[i][j]) + (x[i][k] - coordenada) <= t[i][j][k]);
                model.addConstr(t[i][j][k] <= (x[i][k] - coordenada) + M * (1 - y[i][j]));
            }
        }
    }

    for (int i = 0; i < numS; i++) {
        for (int j = numT, indexJ = i + 1; j < y[i].size(); j++, indexJ++) {
            for (int k = 0; k < DIMENSAO; k++) {
                
                model.addConstr((-1) * M * y[i][j] <= t[i][j][k]);
                model.addConstr(t[i][j][k] <= M * y[i][j]);

                model.addConstr((-1) * M * (1 - y[i][j]) + (x[i][k] - x[indexJ][k]) <= t[i][j][k]);
                model.addConstr(t[i][j][k] <= (x[i][k] - x[indexJ][k]) + (1 - y[i][j]) * M);
            }
        }
    }
}

void criarRestricoesZModeloMaculan(
    int numS,
    int numT,
    GRBModel &model,
    std::vector<std::vector<GRBVar>> &z,
    std::vector<std::vector<std::vector<GRBVar>>> &t
) {
    for (size_t i = 0; i < z.size(); i++) {
        for (size_t j = 0; j < z[i].size(); j++) {
            GRBQuadExpr expr = 0;
            for (int k = 0; k < DIMENSAO; k++) {
                expr += t[i][j][k] * t[i][j][k];
            }
            model.addQConstr(z[i][j] * z[i][j] >= expr);
        }
    }

    for (int i = 0; i < numS; i++) {
        for (int j = numT; j < t[j].size(); j++) {
            GRBQuadExpr expr = 0;
            for (int k = 0; k < DIMENSAO; k++) {
                expr += t[i][j][k] * t[i][j][k];
            }
            model.addQConstr(z[i][j] * z[i][j] >= expr);
        }
    }
}

GRBModel criarModeloMaculan(
    GRBEnv &env, 
    int numT,
    std::vector<Coordenada> &coordenadas // poderá ser necessário mudar para vetor de coordenadas
) {
    GRBModel model = GRBModel(env);
    const int numS = numT - 2;
    const int M = calcularM(coordenadas);

    std::vector<std::vector<GRBVar>> y, z, x;
    std::vector<std::vector<std::vector<GRBVar>>> t;

    criarVariaveisModeloMaculan(numS, numT, model, y, z, x, t);

    criarRestricoesYModeloMaculan(numS, numT, model, y);

    criarRestricoesTModeloMaculan(numS, numT, model, M, coordenadas, y, x, t);

    criarRestricoesZModeloMaculan(numS, numT, model, z, t);

    return model;
}