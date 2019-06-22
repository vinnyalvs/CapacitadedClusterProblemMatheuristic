//
// Created by edson on 09/10/18.
//

#ifndef TRABALHO_IC_DEFINICAO_GRUPOS_HEURISTIC_H
#define TRABALHO_IC_DEFINICAO_GRUPOS_HEURISTIC_H

#include "Solution.h"
#include "Input.h"
#include <string>
#include "../ExactSources/ProblemSolver.h"
#include <random>
#include <algorithm>
#include <utility>
#include <set>
#include <map>
#include <time.h>
#include <cfloat>

using namespace std;
class Heuristic {
public:

    Heuristic(Input* input);
    //~Heuristic();
    void constructive(double alpha, unsigned long seed, double tRemaining);
    //void greedyRandomizedReactive(int alphaRR, int betaRR, double tRemaining, unsigned long seed);
	void greedyRandomizedReactive(int alphaRR, int betaRR, int numIterations, unsigned long seed);
	void buildSolFromFile(vector<vector<int>> clusters);
	void localSearch(double alpha);
	void construtivo();
	void BestVG(Group * group, vector<Node>* nodesNotInSol);
	void BestGV(int node, vector<Group>* groupList);
	void swapShake();
	void shake();
	void trade();
	void trade2();
	void runSolver();
	unsigned int getWorstNode(Group *group);
    clock_t tInicioLeitura, tFimLeitura;
	vector <Node> nodeInSol;
    Solution* solution;
    vector <Solution*> solutions;
    mt19937_64 random;
    Input *input;
    Graph* g;
    ProblemSolver *pSolver;
    void greedyRandomized(double alpha);
    void greedyRandomized2(double alpha);
	double calculateGain(Group * G, unsigned int nodeID);
	double calculateGain(Group * group, unsigned int nodeId, int aux);
	unsigned int getBigNodeNotInSol(vector <Node> *nodesInSol, vector <Node> * nodesNotInSol, int *aux);
	bool checkLowerBound(vector<Group> *groupList);
private:
    //primeira fase de construcao: definir cada grupo inserindo o minimo de nos para atingir o lowerBound no grupo


    void phase2(vector <Group> *groupList, vector <Node> *nodelist, double alpha);
	void phase3(vector <Group> *groupList, vector <Node> *nodelist, double alpha);
    int swapFix();
    int fixSolution();






    int selectAlpha(double **alphas, int alphaRR, double rd);

    void updateAlphaProbability(double **alphas, int alphaRR, double bestCost);
};


#endif //TRABALHO_IC_DEFINICAO_GRUPOS_HEURISTIC_H

