#include "../headers/ProblemSolver.h"

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

ProblemSolver::~ProblemSolver(){
    delete env;
    for(int i=0;i<solutions.size();i++){
        delete solutions[i];
    }
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

void ProblemSolver::addSolution(Solution *solution)
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
	
    cout << "Chamando Resolvedor" << endl;
	buildProblem();




}
void ProblemSolver::buildProblem() {
	Model * newModel = NULL;
	Environment *env = new Environment(solver);
	newModel = env->getModel();
	int indCnstNumClusters;
	int numClusterBound = 2; // Numero de clusters é fixo
	clock_t t = clock();
	const int numAllClusters = solutions.size() * numClusters;

	vector <int> exprs;
	exprs.assign(numObjs, 0);


	string op = (numClusterBound == 1 ? "<=" : (numClusterBound == 2 ? "=" : ">="));
	cout << numObjs << endl;
	if (p.fixedNumClusters) {
		newModel->addConstraint(numClusters, op, "numClusters", numClusters);
		indCnstNumClusters = newModel->getNumConstraints() - 1;
	}
	for (int j = 0; j < numObjs; j++) {
		newModel->addConstraint(1, "=", "ConstrObject" + std::to_string(j), 1);
	}
	//system("clear");
	int countCluster = 0;
	cout << "------------" << endl;
	cout << "Sol:" << solutions.size() << endl;

	for (int i = 0; i < solutions.size(); i++) {
		//cout << i << endl;
		vector <int> costs;
		vector <Group> clusters = solutions[i]->groupList;
		for (auto c : clusters) {
			costs.push_back(c.cost);
		}
		for (int j = 0; j < numClusters; j++) {
			newModel->addVar(1, costs[j], "cluster" + std::to_string(newModel->getNumVars()), "int", 0);
			int indVar = newModel->getNumVars() - 1;
			for (int k = 0; k < clusters[j].nodeList.size(); k++) {
				unsigned int indCnstr = clusters[j].nodeList[k] + 1;
				newModel->setConstraintCoeffs(1, indCnstr, indVar);
			}
			if (p.fixedNumClusters)
				newModel->setConstraintCoeffs(1, indCnstNumClusters, indVar);
		}
	}

	newModel->buildModel("maximize");
	vector <int> x = newModel->getVarsInSol();
	int solution, clusterSol;
	for (int c = 0; c<x.size(); c++) {
		solution = floor(x[c] / numClusters);
		clusterSol = x[c] - ((solution)*numClusters);
		vector <Group> groups = solutions[solution]->groupList;
		clusters.push_back(groups[clusterSol]);

	}
	cout << (clock() - t) / (double)CLOCKS_PER_SEC << endl;

}
void ProblemSolver::newBuildProblem() {
	
	GRBEnv* env = 0;
	GRBVar* open = 0;
	int aux = 0;
	GRBLinExpr exprFixed = 0;
	clock_t t = clock();
	try {
		const int numAllClusters = solutions.size() * numClusters;
		// Model
		env = new GRBEnv();
		GRBModel model = GRBModel(*env);
		model.set(GRB_StringAttr_ModelName, "CCP");
		open = model.addVars(numAllClusters, GRB_BINARY);
		//open = model.addVars(numAllClusters, GRB_INTEGER);
		vector <double> costs;
		for (int i = 0; i < solutions.size(); i++) {
			vector <Group> clusters = solutions[i]->groupList;
			for (int j = 0; j < numClusters; j++) {
				costs.push_back(clusters[j].cost);
			}
		}

		for (int i = 0; i < numAllClusters; i++) {
			ostringstream cname;
			cname << "cluster" << i;
			//open[i].set(GRB_DoubleAttr_UB, 1.0);
			//open[i].set(GRB_DoubleAttr_LB, 0.0);
			open[i].set(GRB_DoubleAttr_Obj, costs[i]);
			open[i].set(GRB_StringAttr_VarName, cname.str());
			exprFixed += open[i];
		}

		ostringstream vname;
		vname << "NumClusterFixed";
		model.addConstr(exprFixed == numClusters, vname.str());
		cout << "fim " << endl;

		IloExpr fixedNumClusters = 0;
		for (int i = 0; i < numObjs; i++) {
			GRBLinExpr expr = 0;
			ostringstream cname;
			cname << "Objeto" << i;
			for (int s = 0; s < solutions.size(); s++) {
				expr += open[solutions[s]->getGroup(i) + 8*s] ;
			}
			model.addConstr(expr == 1, cname.str());
		}
		cout << "Tempo: ";
		cout << (clock() - t) / (double)CLOCKS_PER_SEC << endl;
		model.write("file.lp");
		model.write("model.mps");
		model.update();
		model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE );
		model.optimize();
		
	}
	catch (GRBException e) {
		cout << "Error code = " << e.getErrorCode() << endl;
		cout << e.getMessage() << endl;
	}


}



vector<Group> ProblemSolver::getClusters()
{
	 return clusters ;
}


