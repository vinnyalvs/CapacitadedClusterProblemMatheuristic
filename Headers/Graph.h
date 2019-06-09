//
// Created by edson on 09/10/18.
//

#ifndef TRABALHO_IC_DEFINICAO_GRUPOS_GRAPH_H
#define TRABALHO_IC_DEFINICAO_GRUPOS_GRAPH_H

#include "Node.h"

class Graph {
public:
    Graph(unsigned int order);
    ~Graph();

    void printNodeEdges(unsigned int id);

    vector <Node> nodes;
    double** edges;
    unsigned int order;
};


#endif //TRABALHO_IC_DEFINICAO_GRUPOS_GRAPH_H
