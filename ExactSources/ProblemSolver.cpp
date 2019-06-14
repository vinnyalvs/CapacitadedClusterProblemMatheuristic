#include "ProblemSolver.h"

enum {
	EXATO, 
	FRAC
};



ProblemSolver::ProblemSolver(int solver, int numObjs, int numClusters)
{
	this->solver = solver;
	this->numObjs = numObjs;
	this->numClusters = numClusters;
	this->env = new Environment(solver);
	p.fixedNumClusters = true; //Por enquanto
	cout << "Iniciando Solver" << endl;

	
}




void ProblemSolver::setEnvironment(Environment *env)
{
	this->env = env;
}

Environment * ProblemSolver::getEnvironment()
{
	return env;
}

void ProblemSolver::setSolver(int solver)
{
	this->solver = solver;
}

void ProblemSolver::addSolution(Solution * solution)
{
	solutions.push_back(solution);
}


Solution *ProblemSolver::getBestSolution()
{
	return bestSolution;
}

int ProblemSolver::getBestSolutionId()
{
	return bestSolutionId;
}

void ProblemSolver::setParams(Param p)
{
	this->p = p;
}

void ProblemSolver::solveProblem()
{
	// build Master Problem
	//mProblem.setEnvironment(env);

	buildProblem();

/*	objByCluster.assign(numObjs, 0);
	
	clusters = mProblem.getClusters();

	for (int i = 0; i < clusters.size(); i++) {
		for (int j = 0; j < clusters[i].size(); j++) {
			objByCluster[clusters[i][j] - 1] = i;
		}
	}  */

	cout << "a" << endl;
}
void ProblemSolver::buildProblem(){
	Environment *newEnv = new Environment(1);
	Model *newModel = newEnv->getMdlCplex();
	int indCnstNumClusters;
	int numClusterBound = 2; // Numero de clusters é fixo
	
	const int numAllClusters = solutions.size() * numClusters;

	vector <int> exprs;
	exprs.assign(numObjs, 0);

	vector <double> costClusters;
	string op = (numClusterBound == 1 ? "<=" : (numClusterBound == 2 ? "=" : ">="));

	if (p.fixedNumClusters) {
		newModel->addConstraint(numClusters, op, "numClusters", numClusters);
		indCnstNumClusters = newModel->getNumConstraints() - 1;
	}
	for (int j = 0; j < numObjs; j++) {
		newModel->addConstraint(1, "=", "ConstrObject" + std::to_string(j), 1);
	}
	system("cls");
	int countCluster = 0;
	for (int i = 0; i < solutions.size(); i++) {
		//cout << i << endl;
		vector <double> costs;
		vector <Group> clusters = solutions[i]->groupList;
		for(auto c: clusters){
			costs.push_back(c.cost);
		}
		for (int j = 0; j < numClusters; j++) {
			newModel->addVar(1, costs[j], "cluster" + std::to_string(newModel->getNumVars()), "int", 0);
			int indVar = newModel->getNumVars() - 1;
			for (int k = 0; k < clusters[j].nodeList.size(); k++) {
				unsigned int indCnstr = clusters[j].nodeList[k];
				newModel->setConstraintCoeffs(1, indCnstr, indVar);
			}
			if (p.fixedNumClusters)
				newModel->setConstraintCoeffs(1, indCnstNumClusters, indVar);
			}
}

	newModel->buildModel("maximize");
	vector <int> x = newModel->getVarsInSol();
	int solution, clusterSol;
	for(int c=0;c<x.size();c++){
		solution = floor(x[c]/numClusters);
		clusterSol = x[c] - ((solution)*numClusters);
		vector <Group> groups = solutions[c]->groupList;
		clusters.push_back(groups[clusterSol]);

	} 

}

vector<Group> ProblemSolver::getClusters()
{
	 return clusters ;
}

ProblemSolver::~ProblemSolver()
{
	delete env;
}

