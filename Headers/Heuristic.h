//
// Created by edson on 09/10/18.
//

#ifndef TRABALHO_IC_DEFINICAO_GRUPOS_HEURISTIC_H
#define TRABALHO_IC_DEFINICAO_GRUPOS_HEURISTIC_H

#include "Solution.h"
#include "Input.h"
#include <random>
#include <algorithm>
#include <set>
#include <map>
#include <time.h>
#include <cfloat>

class Heuristic {
public:
    Heuristic(Input* input);
    void constructive(double alpha, unsigned long seed, double tRemaining);
    //void greedyRandomizedReactive(int alphaRR, int betaRR, double tRemaining, unsigned long seed);
	void greedyRandomizedReactive(int alphaRR, int betaRR, int numIterations, unsigned long seed);
	void localSearch(double alpha);
    clock_t tInicioLeitura, tFimLeitura;

    Solution* solution;
    mt19937_64 random;
    Input *input;
    Graph* g;
    void greedyRandomized(double alpha);
	double calculateGain(Group * G, unsigned int nodeID);
	bool checkLowerBound(vector<Group> *groupList);
private:
    struct changeGroup{
        int groupId;
        int nodeId;
        int removedPos;
        double newWeight;
        double newWeight2;
    };

    //primeira fase de construcao: definir cada grupo inserindo o minimo de nos para atingir o lowerBound no grupo


    void phase2(vector <Group> groupList, vector <Node> *nodelist, double alpha);
	void phase3(vector <Group> *groupList, vector <Node> nodelist);
    int swapFix();
    int fixSolution();






    int selectAlpha(double **alphas, int alphaRR, double rd);

    void updateAlphaProbability(double **alphas, int alphaRR, double bestCost);
};


#endif //TRABALHO_IC_DEFINICAO_GRUPOS_HEURISTIC_H

