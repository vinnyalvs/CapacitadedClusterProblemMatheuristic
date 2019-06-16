//
// Created by edson on 09/10/18.
//

#include "../headers/Heuristic.h"

Heuristic::Heuristic(Input *input)
{
	solution = new Solution;
	this->input = input;
	this->g = input->g;
	pSolver = new ProblemSolver(1,input->g->order,input->K);
}

void Heuristic::constructive(double alpha, unsigned long seed, double tRemaining)
{
	random.seed(seed);
	greedyRandomized(alpha);
	cout << "Guloso: " << solution->calculateCost() << endl;


}

void Heuristic::greedyRandomized(double alpha)
{
	solution = new Solution;
	vector <Node> nodes = g->nodes;
	vector <Group> groupList;
	//Talvez ordenar nós por peso, colocar mais pesados primeiro
	//Fase 1, selleciona K vértices alatórios e coloca em K grupos diferentes
	for (int i = 0; i < input->K; i++) {
		Group G;
		G.weight = 0.0;
		G.cost = 0.0;
		G.id = i;
		groupList.push_back(G);
		unsigned int nodeId = rand() % nodes.size();
		nodeId = nodes[nodeId].id;
		groupList[i].insertNode(nodeId);
		int j;
		for (j = 0; j < nodes.size(); j++) {
			if (nodes[j].id == nodeId)
				break;
		}
		if (j == nodes.size())
			nodes.erase(nodes.begin() + (j - 1));
		else
			nodes.erase(nodes.begin() + j);
	}
	//Fase 2
	bool control = false;
	int count = 0;
	while (!control) {
		int bestGroup = -1;
		double bestGain = 0.0;
		unsigned int nodeId = rand() % nodes.size();
		nodeId = nodes.at(nodeId).id;
		vector <Group> groupCandidates = groupList;
		for (int k = 0; k < (int)(groupCandidates.size()); k++) {
		//for (int k = 0; k < (int)(groupCandidates.size() * alpha);k++) {
		//	if ((groupCandidates[k].weight + g->nodes[nodeId].weight) <  input->upperB) {
			if ((groupCandidates[k].weight) <  input->lowerB) {
				double gain = calculateGain(&groupCandidates[k], nodeId);
				if (gain > bestGain) {
					bestGroup = k; //groupCandidates[groupId].id;
					bestGain = gain;
				}
			}
		}
		if (bestGroup != -1) {
			groupCandidates[bestGroup].insertNode(nodeId);
			int i;
			for (i = 0; i<nodes.size(); i++)
				if (nodes.at(i).id == nodeId)
					break;
			nodes.erase(nodes.begin() + i);
		}
		if (checkLowerBound(&groupCandidates)) {
			control = true;
		}
		else if (nodes.size() == 0) {
			control = true;
		}
		groupList = groupCandidates;
	}
	solution->groupList = groupList;
	//Fase 3
	vector <Group> auxCandidates;
	int auxWeightControl = 0;
	int bestGroup = -1;
	unsigned int groupId;
	std::sort(groupList.begin(), groupList.end(), [](Group lhs, Group rhs)
	{
		return lhs.id < rhs.id;
	});
	if (nodes.size() != 0)
		control = false;
	while (!control) {
		double bestGain = 0.0;
		unsigned int nodeId = rand() % nodes.size();
		nodeId = nodes[nodeId].id;
		vector <Group> groupCandidates = groupList;
		//ordenar pelo ganho
		for (auto c : groupCandidates) {
			if ((c.weight + g->nodes[nodeId].weight) <=  input->upperB) {
				double gain = calculateGain(&c, nodeId);
				if (gain > bestGain) {
					bestGroup = c.id; //groupCandidates[groupId].id;
					bestGain = gain;
				}
			}
		}
		if (bestGroup != -1) {
			groupCandidates[bestGroup].insertNode(nodeId);
			int i;
			for (i = 0; i<nodes.size(); i++)
				if (nodes.at(i).id == nodeId)
					break;
			nodes.erase(nodes.begin() + i);
		}
		if (nodes.size() == 0) {
			control = true;
		}
		//	solution->groupList = groupCandidates;
		groupList = groupCandidates;
		//	cout << "Cost: " << solution->calculateCost() << endl;;
	}
	if(nodes.size() != 0)
		for (int i = 0; i < auxCandidates.size(); i++)
			groupList.push_back(auxCandidates[i]);
	solution->groupList = groupList;
		//cout << "Cost: " << solution->calculateCost() << endl;
	//	cout << "V: " << solution->isFeasible(input->lowerB,input->upperB) << endl;;

}

double Heuristic::calculateGain(Group *group, unsigned int nodeId) {
	double gain = 0.0;
	for (auto n : group->nodeList) {
		gain += g->edges[n][nodeId];
	}
	return gain/group->weight;
}

bool Heuristic::checkLowerBound(vector <Group> *groupList) {
	for (auto c : *groupList) {
		if (c.weight < input->lowerB) {
			return false;
		}
	}
	return true;
}

unsigned int Heuristic::getWorstNode(Group *group){
    unsigned int worstId;
    double worstGain;
    worstId = group->nodeList[0];
    worstGain = calculateGain(group,worstId);
    for(int j=1; j< group->nodeList.size(); j++ ){
        double gain = calculateGain(group,group->nodeList[j]);
        if(gain < worstGain){
            worstId = group->nodeList[j];
            worstGain = gain;
        }
    }
    return worstId;
}

void Heuristic::trade(){
    cout << "LS Trade" << endl;
    vector <Group> groupList = solution->groupList;
	bool control = false;
	double cost = solution->cost;
	int count=0;
	while(!control){
        double solCost = cost;
        count++;
        unsigned int A = rand() % groupList.size();
        unsigned int B = rand() % groupList.size();
        while(A != B)
            B = rand() % groupList.size();
        Group ga = groupList[A];
        Group gb = groupList[B];
        unsigned int nodeA = getWorstNode(&ga);
        unsigned int nodeB = getWorstNode(&gb);
        double bestGain=0;
        int groupId=-1;
        double oldWA = ga.weight - g->nodes[nodeA].weight + g->nodes[nodeB].weight ;
        double oldWB = gb.weight - g->nodes[nodeB].weight + g->nodes[nodeA].weight ;
       // cout << oldW<< endl;
        if(!((oldWA) < input->lowerB) && !((oldWB) < input->lowerB) && !((oldWA) > input->upperB) && !((oldWB) > input->upperB) ){
                   double newCA = ga.cost - calculateGain(&ga,nodeA); + calculateGain(&ga,nodeB);
                   double newCB = gb.cost - calculateGain(&gb,nodeB); + calculateGain(&gb,nodeA);
                    cout << "O " <<   (ga.cost + gb.cost) << endl;
                   cout << "N " << ( newCA + newCB) << endl;
                   if( ( (ga.cost + gb.cost) < ( newCA + newCB) ) ) {
                         cout << "entrou" << endl;
                          ga.removeNode(nodeA);
                          ga.insertNode(nodeB);
                          gb.insertNode(nodeA);
                          gb.removeNode(nodeB);
                          groupList[A] = ga;
                          groupList[B] = gb;
                          solution->groupList = groupList;
                          solCost = solution->calculateCost();
                          cout << "troucou" << endl;
                   }
        }
        if(count > 50)
            control = true;
        else
            cost = solCost;

	}
}

void Heuristic::localSearch(double alpha)
{
    //1-opt
	//Sortear (todos os nos) no e colocar no melhor possivel enquanto ha melhora (ou duas vezes)
	//pre calcular(x e y)
	//Tirar pior cada cluster e colocar no melhor
	vector <Group> groupList = solution->groupList;
	vector <Node> nodes = g->nodes;
	shuffle(nodes.begin(),nodes.end(),std::default_random_engine(13));
    bool control = false;
    int count =0;
    double cost = solution->cost;
    while(!control){
   // cout << count++ << endl;
    double solCost = cost;
	for(auto node:nodes){
        int oldGroupId = solution->getGroup(node.id);
        double oldCost = groupList[oldGroupId].cost;
        double oldWeight = groupList[oldGroupId].weight;
        double bestGain=0;
        int groupId=-1;
        double newWeight = oldWeight - node.weight;
       // cout << oldW<< endl;
        if(!(newWeight < input->lowerB)){
            for(auto c: groupList){
                if(!((c.weight + node.weight) > input->upperB) && c.id != oldGroupId){
                   double gain = calculateGain(&c,node.id);
                   if(gain > bestGain && ( (c.cost + gain) > oldCost ) )
                        groupId = c.id;
                        bestGain = gain;
                }
            }
        }
        if(groupId != -1){
          //  cout << "troucou" << endl;
            groupList[oldGroupId].removeNode(node.id);
            groupList[groupId].insertNode(node.id) ;
            solution->groupList = groupList;
            solCost = solution->calculateCost();
          //  cout << solCost<< endl;
        }
	}
    if(solCost == cost)
        control = true;
    else
        cost = solCost;
    }

}

void Heuristic::localSearch3(double alpha){
   // system("cls");
    //cout << "LS3" << endl;
    vector <Group> groupList = solution->groupList;
	bool control = false;
	double cost = solution->cost;
	int count=0;
	while(!control){
        double solCost = cost;
        count++;
        std::sort(groupList.begin(), groupList.end(), [](Group lhs, Group rhs)
    {
		return lhs.weight > rhs.weight;
	});
        unsigned int A = rand() % (int)(groupList.size() * alpha);
        Group ga = groupList[A];
        Group gb;
        A = ga.id;
        unsigned int nodeA = getWorstNode(&ga);
        double bestGain=0;
        int groupId=-1;
        double oldW = ga.weight - g->nodes[nodeA].weight;
       // cout << oldW<< endl;
        if(!((oldW) < input->lowerB)){
            for(auto c: groupList){
                if(!((c.weight + g->nodes[nodeA].weight) > input->upperB) && c.id != ga.id){
                   double gain = calculateGain(&c,nodeA);
                   if(gain > bestGain && ( (c.cost + gain) > ga.cost ) )
                        groupId = c.id;
                        bestGain = gain;
                }
            }
        }
        if(groupId != -1){
        //    cout << "troucou" << endl;
            std::sort(groupList.begin(), groupList.end(), [](Group lhs, Group rhs){
                return lhs.id < rhs.id;
            });
            groupList[A].removeNode(nodeA);
            groupList[groupId].insertNode(nodeA) ;
            solution->groupList = groupList;
         //  cout << solution->calculateCost() << endl;
            solCost = solution->calculateCost();
        }
        if(count > 100)
            control = true;
        else
            cost = solCost;

	}
}

void Heuristic::localSearch2(){
    cout << "LS2" << endl;
    vector <Group> groupList = solution->groupList;
    std::sort(groupList.begin(), groupList.end(), [](Group lhs, Group rhs)
    {
		return lhs.id < rhs.id;
	});
	bool control = false;
	double cost = solution->cost;
	int count=0;
	while(!control){
        double solCost = cost;
        count++;
        unsigned int A = rand() % groupList.size();
        Group ga = groupList[A];
        Group gb;
        unsigned int nodeA = getWorstNode(&ga);
        double bestGain=0;
        int groupId=-1;
        double oldW = ga.weight - g->nodes[nodeA].weight;
       // cout << oldW<< endl;
        if(!((oldW) < input->lowerB)){
            for(auto c: groupList){
                if(!((c.weight + g->nodes[nodeA].weight) > input->upperB) && c.id != ga.id){
                   double gain = calculateGain(&c,nodeA);
                       if(gain > bestGain && ( (c.cost + gain) > ga.cost ) ) {
                            groupId = c.id;
                            bestGain = gain;
                    }
                }
            }
        }
        if(groupId != -1){
           // cout << "troucou" << endl;
            groupList[A].removeNode(nodeA);
            groupList[groupId].insertNode(nodeA) ;
            solution->groupList = groupList;
           // cout << solution->calculateCost() << endl;
            solCost = solution->calculateCost();
        }
        if(count > 500)
            control = true;
        else
            cost = solCost;

	}
}


int Heuristic::swapFix()
{


	return 0;
}

int Heuristic::fixSolution()
{
	return 1;
}

void Heuristic::runSolver(){
/*Environment env = new Environment(solver);
Param p;
p.fixedNumClusters = true;
Model *newModel = env->getMdlCplex();
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
    //system("clear");
	int countCluster = 0;
	cout << "------------" << endl;
	cout << "Sol:" << solutions.size() << endl;

	for (int i = 0; i < solutions.size(); i++) {
		//cout << i << endl;
		vector <double> costs;
        cout << i << " " << solutions[i]->groupList.size() << " " << solutions[i]->cost <<  endl;
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

	} */ ///
	pSolver->solveProblem();


}

void Heuristic::greedyRandomizedReactive(int alphaRR, int betaRR, int numIterations, unsigned long seed) {

	clock_t tBegin, tActual;
	int iterationCount = 0;
	int deletedCount = 0;
	double time;
	tBegin = clock();
	random.seed(seed);
	double **alphas = new double *[alphaRR]; //Alocando o vetor dos alphas
	double bestCost = DBL_MIN;
	double cost;

	Solution *bestS = nullptr;

	//Criando os alphas baseado na quantidade passada (alphaRR)
	for (int i = 0; i < alphaRR; i++) {
		alphas[i] = new double[4];
		alphas[i][0] = (1.0 / alphaRR) * (i + 1.0); //valor do alpha
		alphas[i][1] = 1.0 / alphaRR;               //probabilidade do alpha
		solution = nullptr;
		while (solution == nullptr)
			greedyRandomized(alphas[i][0]);              //inicializando o somatorio para nao zerar a probabilidade de escolha
		if (solution->isFeasible(input->lowerB, input->upperB)) {
			alphas[i][2] = solution->calculateCost();  //soma do valor das execucoes
			alphas[i][3] = 0;                         //quantidade de execucoes
			//pSolver->addSolution(solution);
		}
		else
			deletedCount++;


	}

	int ind = 0;              //variaveis auxiliares a escolha do alpha
	uniform_real_distribution<double> randomFloat(0.0, 1.0);
	default_random_engine re;

	//loop de execucao
	while (iterationCount < numIterations)
	{
		/*
		for(int i = 0; i < alphaRR; ++i)
		cout << alphas[i][1] << " ";
		cout << endl;
		*/

		ind = selectAlpha(alphas, alphaRR, randomFloat(re));

        cout << iterationCount << endl;
		greedyRandomized(alphas[ind][0]);
        localSearch2();
        localSearch(alphas[ind][0]);
        localSearch3(alphas[ind][0]);

		if (!solution->isFeasible(input->lowerB, input->upperB)) {
			//delete solution;
			deletedCount++;
			break;
		}
		pSolver->addSolution(solution);
		iterationCount++;



	//	cost = solution->calculateCost();
		cout << "S: " << solution->groupList.size() << endl;;
       cout << solution->calculateCost() << endl;
        cost = solution->calculateCost();
        cout << cost << endl;
		if (cost > bestCost && cost != 0) {
			//delete bestS;
			bestS = solution;
			bestCost = cost;
		}
		//else delete solution;
		alphas[ind][2] += cost;
		alphas[ind][3]++;
        cout << "---------" << endl;
		///COLOCAR PARA ATUALIZAR A CADA BETA SEGUNDOS
		updateAlphaProbability(alphas, alphaRR, bestCost);

		tActual = (clock() - tBegin) / (CLOCKS_PER_SEC);
		time = (double)tActual;
		cout << "Deletadas" << deletedCount << endl;
		cout << "tempo: " << time << "s" << endl;
	}

	for (int i = 0; i < alphaRR; ++i)
		delete[] alphas[i];
	delete[] alphas;


	solution = bestS;
}

void Heuristic::updateAlphaProbability(double **alphas, int alphaRR, double bestCost)
{
	double *qi = new double[alphaRR], qj = 0.0;

	/*
	for (int i = 0; i < alphaRR; ++i)
	cout << "[" << alphas[i][1] << "," << alphas[i][2] << "," << alphas[i][3] << "]" << endl;
	cout << "b:" << bestCost << endl;
	*/

	for (int i = 0; i < alphaRR; i++)
	{
		qi[i] = pow(((alphas[i][2] / (alphas[i][3] + 1))) / bestCost, 1.0);
		qj += qi[i];
	}

	for (int i = 0; i < alphaRR; ++i)
		alphas[i][1] = qi[i] / qj;

	for (int i = 0; i < alphaRR; ++i)
	{
		//delete solution;
		alphas[i][3] = 0;
		solution = nullptr;
		while (solution == nullptr)
			greedyRandomized(alphas[i][0]);
		alphas[i][2] = solution->calculateCost();
	}

	delete qi;

}


int Heuristic::selectAlpha(double **alphas, int alphaRR, double rd)
{
	double sum = 0.0;
	int _return = -1;
	for (int j = 0; j < alphaRR; j++)
	{
		sum += (alphas[j][1]);
		if (sum >= rd)
		{
			_return = j;
			break;
		}
	}
	return _return;
}



