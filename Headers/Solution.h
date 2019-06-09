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
    unsigned int* nodeGroupRelation;
    void printSolution();
    void insertNode(unsigned int groupId, unsigned int id);
    void insertNodeBack(unsigned int id);
    void swapNodes(unsigned int groupId, unsigned long removedPosition, unsigned int insertedId);

    double calculateCost();
    bool isFeasible(double lowerB,double upperB);



};


#endif //TRABALHO_IC_DEFINICAO_GRUPOS_SOLUTION_H
