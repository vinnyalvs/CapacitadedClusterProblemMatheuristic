//
// Created by edson on 09/10/18.
//

#ifndef TRABALHO_IC_DEFINICAO_GRUPOS_NODE_H
#define TRABALHO_IC_DEFINICAO_GRUPOS_NODE_H

#include <utility>
#include <iostream>
#include <vector>

using namespace std;

typedef struct pair<unsigned int, double> Edge;

class Node {
public:
    Node(unsigned int id);

    vector<Edge> edges;
    unsigned int id;
    unsigned int degree;
    double weight;

    static bool edgeSorter(Edge e1, Edge e2);
};


#endif //TRABALHO_IC_DEFINICAO_GRUPOS_NODE_H
