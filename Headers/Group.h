//
// Created by edson on 11/10/18.
//

#ifndef TRABALHO_IC_DEFINICAO_GRUPOS_GROUP_H
#define TRABALHO_IC_DEFINICAO_GRUPOS_GROUP_H

#include "Graph.h"

using namespace std;

class Group {
public:
    Group();
    Group(unsigned int id);
    ~Group(){};
    void insertNode(unsigned int id);
    void printGroup();

    vector<unsigned int> nodeList;
    double weight;
    double cost;
//	double lowerB;
//	double upperB;
    unsigned int id;
    static Graph* g;
};


#endif //TRABALHO_IC_DEFINICAO_GRUPOS_GROUP_H
