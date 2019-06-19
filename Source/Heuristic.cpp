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
	solution->calculateCost();
		//cout << "Cost: " << solution->calculateCost() << endl;
	//	cout << "V: " << solution->isFeasible(input->lowerB,input->upperB) << endl;;

}

double Heuristic::calculateGain(Group *group, unsigned int nodeId) {
	double gain = 0.0;
	for (auto n : group->nodeList) {
		if(n != nodeId)
			gain += g->edges[n][nodeId];
	}
	return gain/group->weight;
}

double Heuristic::calculateGain(Group *group, unsigned int nodeId, int aux) {
	double gain = 0.0;
	for (auto n : group->nodeList) {
		if (n != nodeId)
			gain += g->edges[n][nodeId];
	}
	return gain;
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
       Solution *backup = new Solution();
    backup->cost = solution->cost;
    backup->groupList = solution->groupList;
    vector <Group> groupList = backup->groupList;
	bool control = false;
	double cost = solution->cost;
	int count=0;
    double solCost = backup->cost;
	while(!control){
        count++;
        unsigned int A = rand() % groupList.size();
        unsigned int B = rand() % groupList.size();
        while(A == B)
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
                   double newCA = ga.cost - calculateGain(&ga,nodeA,0) + calculateGain(&ga,nodeB,0);
                   double newCB = gb.cost - calculateGain(&gb,nodeB,0) + calculateGain(&gb,nodeA,0);
                   if( ( (ga.cost + gb.cost) < ( newCA + newCB) ) ) {
                          ga.removeNode(nodeA);
                          ga.insertNode(nodeB);
                          gb.insertNode(nodeA);
                          gb.removeNode(nodeB);
                          groupList[A] = ga;
                          groupList[B] = gb;
                          backup->groupList = groupList;
                          double c = backup->calculateCost();
                          //solCost = backup->calculateCost();

                   }
        }
       if((solCost >= backup->cost))
            count++;
         else{
            count = 0;
            solCost = backup->cost;
        }
        if(count > 1000)
            control = true;




	}
    if(solution->cost < backup->cost){
        solution->groupList = backup->groupList;
        solution->cost = backup->cost;
    }
}

void Heuristic::trade2(){
	Solution *backup = new Solution();
    backup->cost = solution->cost;
    backup->groupList = solution->groupList;
    vector <Group> groupList = backup->groupList;
	bool control = false;
	double cost = backup->cost;
	int count=0;
	while(!control){
        double solCost = backup->cost;
        count++;
        unsigned int A = rand() % groupList.size();
        unsigned int B = rand() % groupList.size();
        while(A == B)
            B = rand() % groupList.size();
        Group ga = groupList[A];
        Group gb = groupList[B];
        Group aux;
        unsigned int nodeA = getWorstNode(&ga);
        aux = ga;
        aux.removeNode(nodeA);
        unsigned int nodeA2 = getWorstNode(&aux);
        unsigned int nodeB = getWorstNode(&gb);
        double bestGain=0;
        int groupId=-1;
        double oldWA = ga.weight - g->nodes[nodeA].weight  - g->nodes[nodeA2].weight + g->nodes[nodeB].weight ;
        double oldWB = gb.weight - g->nodes[nodeB].weight + g->nodes[nodeA].weight  + g->nodes[nodeA2].weight ;
        if(!((oldWA) < input->lowerB) && !((oldWB) < input->lowerB) && !((oldWA) > input->upperB) && !((oldWB) > input->upperB) ){
                   double newCA = ga.cost - calculateGain(&ga,nodeA,0) - calculateGain(&ga,nodeA2,0) + calculateGain(&ga,nodeB,0);
                   double newCB = gb.cost - calculateGain(&gb,nodeB,0) + calculateGain(&gb,nodeA,0) + calculateGain(&ga,nodeA2,0);
                   if( ( (ga.cost + gb.cost) < ( newCA + newCB) ) ) {
                          ga.removeNode(nodeA);
                          ga.removeNode(nodeA2);
                          ga.insertNode(nodeB);
                          gb.insertNode(nodeA);
                          gb.insertNode(nodeA2);
                          gb.removeNode(nodeB);
                          groupList[A] = ga;
                          groupList[B] = gb;
                          double newCost =0;
                          for (auto c : groupList)
                            newCost += c.cost;
                          if (newCost >= solCost) {
                            backup->groupList = groupList;
                            //solCost = backup->calculateCost();
                            backup->cost = newCost;
                          } else
                            groupList = backup->groupList;
        }
	}
        if((solCost >= backup->cost))
            count++;
         else
            count =0;
        if(count > 1000)
            control = true;
 }
 	if(solution->cost < backup->cost){
        solution->groupList = backup->groupList;
        solution->cost = backup->cost;
    }
}




void Heuristic::localSearch(double alpha)
{
    Solution *backup = new Solution();
    backup->cost = solution->cost;
    backup->groupList = solution->groupList;
	vector <Node> nodes = g->nodes;
	shuffle(nodes.begin(),nodes.end(),std::default_random_engine(13));
    bool control = false;
    int count =0;
    double cost = backup->cost;

    while(!control){
    double solCost = cost;
	vector <Group> groupList = backup->groupList;
	for(auto node:nodes){
        int oldGroupId = backup->getGroup(node.id);
        double oldCost = groupList[oldGroupId].cost;
       // double newCost=0;
        double oldWeight = groupList[oldGroupId].weight;
        double bestGain=0;
        int groupId=-1;
        double newWeight = oldWeight - node.weight;
       // cout << oldW<< endl;
        if(!(newWeight < input->lowerB)){
            for(auto c: groupList){
                if(!((c.weight + node.weight) > input->upperB) && c.id != oldGroupId){
                   double gain = calculateGain(&c,node.id,0);
                   if(gain > bestGain && ( (c.cost + gain) > oldCost ) )
                        groupId = c.id;
                        bestGain = gain;
                        //newCost = c.cost + gain
                }
            }
        }

        if(groupId != -1){
            groupList[oldGroupId].removeNode(node.id);
            groupList[groupId].insertNode(node.id) ;
			double newCost = 0;
			for (auto c : groupList)
				newCost += c.cost;
			if (newCost >= solCost) {
				backup->groupList = groupList;
				solCost = backup->calculateCost();
			}
        }
	}
    if(solCost <= cost)
        control = true;
    else
        cost = solCost;
    }
    if(solution->cost < backup->cost){
        solution->groupList = backup->groupList;
        solution->cost = backup->cost;
    }

}




void Heuristic::localSearch2(){
   // cout << "LS2" << endl;
	vector <Group> groupListAux = solution->groupList;
	std::sort(groupListAux.begin(), groupListAux.end(), [](Group lhs, Group rhs)
	{
		return lhs.id < rhs.id;
	});
	bool control = false;
	double cost = solution->cost;
	int count=0;
	while(!control){
		vector <Group> groupList = groupListAux;
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
                   double gain = calculateGain(&c,nodeA,0);
                       if(gain > bestGain && ( (c.cost + gain) > ga.cost ) ) {
                            groupId = c.id;
                            bestGain = gain;
                    }
                }
            }
        }
        if(groupId != -1){
			double newCost = 0;
            groupList[A].removeNode(nodeA);
            groupList[groupId].insertNode(nodeA) ;
			for (auto c : groupList)
				newCost += c.cost;
			if (newCost >= solCost) {
				solution->groupList = groupList;
				solCost = solution->calculateCost();
			}
        }
        if(count > 10)
            control = true;
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

	pSolver->solveProblem();
	vector <Group> groupList = pSolver->getClusters();
	solution->groupList = groupList;
	cout << "Solução : " << solution->calculateCost() << "Viabilidade " << solution->isFeasible(input->lowerB,input->upperB) <<  endl;
	ofstream f;
    f.open("sols.txt",ios::app);
	f << solution->cost << endl;
	delete pSolver;


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
		localSearch(alphas[ind][0]);
		//if(iterationCount % 2 == 0 )
        trade();
        trade2();


		if (!solution->isFeasible(input->lowerB, input->upperB)) {
			delete solution;
			deletedCount++;
			break;
		}
		pSolver->addSolution(solution);
		iterationCount++;

        cost = solution->calculateCost();
		if (cost > bestCost && cost != 0) {
		//	delete bestS;
			bestS = solution;
			bestS->groupList = solution->groupList;
			bestCost = cost;
		}
		//else delete solution;
		alphas[ind][2] += cost;
		alphas[ind][3]++;
        //cout << "---------" << endl;
		///COLOCAR PARA ATUALIZAR A CADA BETA SEGUNDOS
		updateAlphaProbability(alphas, alphaRR, bestCost);

		tActual = (clock() - tBegin) / (CLOCKS_PER_SEC);
		time = (double)tActual;
		//cout << "Deletadas" << deletedCount << endl;
		//cout << "tempo: " << time << "s" << endl;
	}

	for (int i = 0; i < alphaRR; ++i)
		delete[] alphas[i];
	delete[] alphas;


	solution = bestS;
	cout << "BEST Heuristic: " << solution->calculateCost() << endl;
	ofstream f;
    f.open("solsH.txt",ios::app);
	f << solution->cost << endl;
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



