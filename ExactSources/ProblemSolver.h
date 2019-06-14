#pragma once

#include "Opt.h"
#include "Environment.h"
#include "../headers/Solution.h"
#include "../headers/Input.h"
class ProblemSolver
{
private:
	int solver;
	Environment *env;
	vector <Solution*> solutions;
	Solution *bestSolution;
	int bestSolutionId;
	Param p;
	int numObjs;
	int numClusters;
	vector <Group> clusters;

public:
	ProblemSolver(int solver, int numObjs, int numClusters);
	void setEnvironment(Environment *env);
	Environment *getEnvironment();
	void setSolver(int solver);
	void addSolution(Solution *solution);
	//void findBestSolution();
	Solution *getBestSolution();
	int getBestSolutionId();
	void setParams(Param p);
	void solveProblem();
	void buildProblem();




	vector <Group> getClusters();
	vector<int> objByCluster;

	~ProblemSolver();
};

