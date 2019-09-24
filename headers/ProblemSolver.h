#pragma once

#include "../GurobiCplexInterface/Opt.h"
#include "../GurobiCplexInterface/Environment.h"
#include "../headers/Solution.h"
#include "../headers/Input.h"
#include <ctime>
class ProblemSolver
{
private:
	int solver;
	Environment *env;
	
	Solution *bestSolution;
	int bestSolutionId;
	Param p;
	int numObjs;
	int numClusters;
	vector <Group> clusters;

public:
	vector <Solution*> solutions;
	ProblemSolver(int solver, int numObjs, int numClusters);
	~ProblemSolver();
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
	void newBuildProblem();




	vector <Group> getClusters();
	vector<int> objByCluster;

};
