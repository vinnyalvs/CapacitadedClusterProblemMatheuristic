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
		std::sort(groupCandidates.begin(), groupCandidates.end(), [](Group lhs, Group rhs)
		{
			return lhs.weight < rhs.weight;
		});
		unsigned int groupId = rand() % (int)(groupCandidates.size() * alpha);
		double gain = calculateGain(&groupCandidates[groupId], nodeId);
		if (bestGain < gain && ((groupCandidates[groupId].weight + g->nodes[nodeId].weight) <  input->upperB)) {
			bestGroup = groupId; //groupCandidates[groupId].id;
			bestGain = gain;
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
	//Fase 3
	vector <Group> auxCandidates;
	int auxWeightControl = 0;
	int bestGroup = -1;
	unsigned int groupId;
	if(nodes.size() != 0)
	 control = false;
	while (!control) {
		double bestGain = 0.0;
		unsigned int nodeId = rand() % nodes.size();
		nodeId = nodes[nodeId].id;
		vector <Group> groupCandidates = groupList;

		if (auxWeightControl < 3) {
			std::sort(groupCandidates.begin(), groupCandidates.end(), [](Group lhs, Group rhs)
			{
				return lhs.cost > rhs.cost;
			});
		}
		else {
			std::sort(groupCandidates.begin(), groupCandidates.end(), [](Group lhs, Group rhs)
			{
				return lhs.cost > rhs.cost;
			});
			auxCandidates.push_back(groupCandidates[groupId]);
			auxWeightControl = 0;
			groupCandidates.erase(groupCandidates.begin() + groupId);
		}
		groupId = rand() % (int)ceil((groupCandidates.size() * alpha));
		double gain = calculateGain(&groupCandidates[groupId], nodeId);
		bestGroup = -1;
		if (bestGain < gain && ((groupCandidates[groupId].weight + g->nodes[nodeId].weight) <  input->upperB)) {
			bestGroup = groupId; //groupCandidates[groupId].id;
			bestGain = gain;
		}
		if (bestGroup != -1) {
			groupCandidates[bestGroup].insertNode(nodeId);
			int i;
			for (i = 0; i<nodes.size(); i++)
				if (nodes[i].id == nodeId)
					break;
			nodes.erase(nodes.begin() + i);
		}
		else
			auxWeightControl++;
		if (nodes.size() == 0) {
			control = true;
		}
	//	solution->groupList = groupCandidates;
		groupList = groupCandidates;
	//	cout << "Cost: " << solution->calculateCost() << endl;;
	}
	for (int i = 0; i < auxCandidates.size(); i++)
		groupList.push_back(auxCandidates[i]);
	solution->groupList = groupList;
//	cout << "Cost: " << solution->calculateCost() << endl;
//	cout << "Cost: " << solution->isFeasible(input->lowerB,input->upperB) << endl;;

}

double Heuristic::calculateGain(Group *group,unsigned int nodeId) {
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


void Heuristic::phase2(vector<Group> groupList, vector<Node> *nodes, double alpha)
{
	bool control = false;
	int count = 0;
	while (!control) {
		int bestGroup = -1;
		double bestGain = 0.0;
		unsigned int nodeId = rand() % nodes->size();
		nodeId = nodes->at(nodeId).id;
		vector <Group> groupCandidates = groupList;
		std::sort(groupCandidates.begin(), groupCandidates.end(), [](Group lhs, Group rhs)
		{
			return lhs.weight < rhs.weight;
		});
		unsigned int groupId = rand() % (int)(groupCandidates.size() * alpha);
		double gain = calculateGain(&groupCandidates[groupId], nodeId);
		if (bestGain < gain && ((groupCandidates[groupId].weight + g->nodes[nodeId].weight) <  input->upperB)) {
			bestGroup = groupId; //groupCandidates[groupId].id;
			bestGain = gain;
		}
		if (bestGroup != -1) {
			groupCandidates[bestGroup].insertNode(nodeId);
			int i;
			for (i = 0; i<nodes->size(); i++)
				if (nodes->at(i).id == nodeId)
					break;
			nodes->erase(nodes->begin() + i);
		}
		if (checkLowerBound(&groupCandidates)) {
			control = true;
		}
		else if (nodes->size() == 0) {
			control = true;
		}
		groupList = groupCandidates;
	}
}

int Heuristic::swapFix()
{
  
    return 0;
}

int Heuristic::fixSolution()
{
	/*
    vector<Edge> groupList;
    for (int i = 0; i < input->K; ++i)
        groupList.emplace_back(i, solution->groupList[i].weight);
    sort(groupList.begin(), groupList.end(), Node::edgeSorter);

    vector<unsigned int> nodeList;
    for (unsigned int i = 0 ; i < g->order; ++i)
        if (solution->avaliableNodes[i])
            nodeList.push_back(i);

    vector<unsigned int>::iterator it;

    while (!nodeList.empty())
    {
        it = nodeList.begin() + random() % nodeList.size();
        auto iter = groupList.begin();
        for (; iter != groupList.end(); ++iter)
        {
            if (g->nodes[*it].weight + solution->groupList[iter->first].weight  <= input->upperB)
            {
                solution->insertNode(iter->first, (*it));

                for (auto j : g->nodes[*it].edges)
                {
                    if (solution->avaliableNodes[j.first])
                        if (g->nodes[j.first].weight + solution->groupList[iter->first].weight <= input->upperB)
                            solution->insertNode(iter->first, j.first);
                }

                break;
            }
        }
        if (iter == groupList.end())
            return -1;

        nodeList.clear();
        for (unsigned int i = 0 ; i < g->order; ++i)
            if (solution->avaliableNodes[i])
                nodeList.push_back(i);
    }
    return 1; */
	return 1;
}

void Heuristic::greedyRandomizedReactive(int alphaRR, int betaRR, int numIterations, unsigned long seed) {

    clock_t tBegin, tActual;
    int iterationCount=0;
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

        //solution = nullptr;
        //while (solution == nullptr)
         greedyRandomized(alphas[ind][0]);
		 if (!solution->isFeasible(input->lowerB, input->upperB)) {
			 delete solution;
			 deletedCount++;
			 break;
		}
		 iterationCount++;
			
        cost = solution->calculateCost();

       // localSearch2(1);

        if (cost > bestCost && cost != 0) {
            delete bestS;
            bestS = solution;
            bestCost = cost;
        } else delete solution;
        alphas[ind][2] += cost;
        alphas[ind][3]++;

        ///COLOCAR PARA ATUALIZAR A CADA BETA SEGUNDOS
        updateAlphaProbability(alphas, alphaRR, bestCost);

        tActual = (clock() - tBegin) / (CLOCKS_PER_SEC);
        time = (double) tActual;
		cout << "Deletadas" <<  deletedCount << endl;
        cout << "tempo: " << time << "s" << endl;
    }

    for (int i = 0; i < alphaRR; ++i)
        delete[] alphas[i];
    delete[] alphas;

    solution = bestS;
}

void Heuristic::localSearch(double alpha)
{
	vector <Group> groupList = solution->groupList;
	unsigned int groupId = rand() % groupList.size();
	groupId = groupList[groupId].id;


}



void Heuristic::updateAlphaProbability(double **alphas, int alphaRR, double bestCost)
{
    double *qi = new double [alphaRR], qj= 0.0;

    /*
    for (int i = 0; i < alphaRR; ++i)
        cout << "[" << alphas[i][1] << "," << alphas[i][2] << "," << alphas[i][3] << "]" << endl;
    cout << "b:" << bestCost << endl;
     */

    for(int i = 0; i < alphaRR; i++)
    {
        qi[i] = pow(((alphas[i][2] / (alphas[i][3] + 1)))/ bestCost, 1.0);
        qj += qi[i];
    }

    for (int i = 0; i < alphaRR; ++i)
        alphas[i][1] = qi[i]/qj;

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


int Heuristic::selectAlpha(double **alphas,int alphaRR, double rd)
{
    double sum = 0.0;
    int _return = -1;
    for(int j = 0; j < alphaRR; j++)
    {
        sum += (alphas[j][1]);
        if(sum >= rd)
        {
            _return = j;
            break;
        }
    }
    return _return;
}



