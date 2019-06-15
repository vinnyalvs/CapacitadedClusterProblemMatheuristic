//
// Created by edson on 09/10/18.
//

#include "../headers/Heuristic.h"

Heuristic::Heuristic(Input *input)
{
	solution = new Solution;
	this->input = input;
	this->g = input->g;
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
			if ((c.weight + g->nodes[nodeId].weight) <  input->upperB) {
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

void Heuristic::localSearch(double alpha)
{
    //1-opt
	//Sortear (todos os nos) no e colocar no melhor possivel enquanto ha melhora (ou duas vezes)
	//pre calcular(x e y)
	//Tirar pior cada cluster e colocar no melhor
	vector <Group> groupList = solution->groupList;
	unsigned int groupId = rand() % groupList.size();
	groupId = groupList[groupId].id;


}


void Heuristic::localSearch2(){
    cout << "a" << endl;
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
            //    cout << !((c.weight + g->nodes[nodeA].weight) > input->upperB) << endl;
                if(!((c.weight + g->nodes[nodeA].weight) > input->upperB) && c.id != ga.id){
                   // cout << "entrou " << c.id << endl;
                   double gain = calculateGain(&c,nodeA);
                //    cout << "newCOst " << (c.cost + gain) << endl;
                  // cout << "Cost " << ga.cost << endl;
                   if(gain > bestGain && ( (c.cost + gain) > ga.cost ) )
                        groupId = c.id;
                        bestGain = gain;
                }
            }
        }
        if(groupId != -1){
            cout << "troucou" << endl;
            groupList[A].removeNode(nodeA);
            groupList[groupId].insertNode(nodeA) ;
            solution->groupList = groupList;
         //   cout << solution->calculateCost() << endl;
            solCost = solution->cost;
        }
       /* if(solCost == cost)
            control = true; */
          if(count > 500)
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
			solutions.push_back(solution);
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


		greedyRandomized(alphas[ind][0]);
        localSearch2();
		if (!solution->isFeasible(input->lowerB, input->upperB)) {
			delete solution;
			deletedCount++;
			break;
		}
		iterationCount++;
		solutions.push_back(solution);

		cost = solution->calculateCost();
        cout << cost << endl;

		if (cost > bestCost && cost != 0) {
			delete bestS;
			bestS = solution;
			bestCost = cost;
		}
		else delete solution;
		alphas[ind][2] += cost;
		alphas[ind][3]++;

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



