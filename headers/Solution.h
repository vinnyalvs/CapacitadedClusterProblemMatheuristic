//
// Created by edson on 11/10/18.
//

#ifndef TRABALHO_IC_DEFINICAO_GRUPOS_SOLUTION_H
#define TRABALHO_IC_DEFINICAO_GRUPOS_SOLUTION_H

#include "Group.h"
#include <limits>
#include <algorithm>

class Solution {
public:
    Solution();
    ~Solution();
    vector<Group> groupList;
    void printSolution();
    void insertNode(unsigned int groupId, unsigned int id);
    void insertNodeBack(unsigned int id);
    int getGroup(unsigned int nodeId);

    double cost;
    double calculateCost();
    bool isFeasible(double lowerB,double upperB);



};


#endif //TRABALHO_IC_DEFINICAO_GRUPOS_SOLUTION_H
