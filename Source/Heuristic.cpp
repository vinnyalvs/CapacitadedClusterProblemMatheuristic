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
		nodeId = g->nodes[nodeId].id;
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
	cout << "-------" << endl;
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
	 control = false;
	while (!control) {
		int bestGroup = -1;
		double bestGain = 0.0;
		unsigned int nodeId = rand() % nodes.size();
		nodeId = nodes[nodeId].id;
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
				if (nodes[i].id == nodeId)
					break;
			nodes.erase(nodes.begin() + i);
		}
		if (nodes.size() == 0) {
			control = true;
		}
		groupList = groupCandidates;
	}
	system("cls");
	solution->groupList = groupList;
	cout << solution->isFeasible(input->lowerB, input->upperB);
	cout << solution->calculateCost() << endl;;
}

double Heuristic::calculateGain(Group *group,unsigned int nodeId) {
	double gain = 0.0;
	for (auto n : group->nodeList) {
		gain += g->edges[n][nodeId];
		if (gain < 0)
			cout << "g " << gain << " " << n << nodeId << endl;
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
  /*  set<unsigned int> avaliableNodeList;
    set<unsigned int> possibleGroups;

    set<unsigned int>::iterator selectedGroup;

    unsigned long position;
    vector<unsigned int> nodeList;

    bool preFixAvaliableNodes[g->order];
    for (int i = 0; i < g->order; ++i)
        preFixAvaliableNodes[i] = solution->avaliableNodes[i];

    for (unsigned int i = 0; i < g->order; ++i)
        if (solution->avaliableNodes[i])
            avaliableNodeList.insert(i);

    if (avaliableNodeList.empty())
        return 1;

    for (auto i : avaliableNodeList)
    {
        //adiciona a lista de possiveis grupos se o no da aresta j esta em uma solucao e se adicionar
        //o no i nao causa inviabilidade pelo upperB
        for (auto j : g->nodes[i].edges)
            if (!solution->avaliableNodes[j.first] && solution->nodeGroupRelation[j.first] != UINT32_MAX)
                if(solution->groupList[solution->nodeGroupRelation[j.first]].weight + g->nodes[i].weight <= input->upperB)
                    possibleGroups.insert(solution->nodeGroupRelation[j.first]);

        if (possibleGroups.empty())
            continue;

        position = random() % possibleGroups.size();

        selectedGroup = possibleGroups.begin();
        for (int k = 0; k < position; ++k)
            ++selectedGroup;

        nodeList.clear();
        nodeList = solution->groupList[*selectedGroup].nodeList;

        while (!nodeList.empty())
        {
            position = random() % nodeList.size();
            if (solution->groupList[*selectedGroup].weight - g->nodes[position].weight +
                        g->nodes[i].weight <= input->upperB)
            {
                solution->swapNodes(*selectedGroup, position, i);
                break;
            }
            else
                nodeList.erase(nodeList.begin() + position);
        }
    }

    int i = 0;
    for (i = 0; i < g->order && preFixAvaliableNodes[i] == solution->avaliableNodes[i]; ++i);
    if (i == g->order)
        return fixSolution();

    phase2(); */
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

void Heuristic::greedyRandomizedReactive(int alphaRR, int betaRR, double tRemaining, unsigned long seed) {

    clock_t tBegin, tActual;
    double time=0;
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
        alphas[i][2] = solution->calculateCost();        //soma do valor das execucoes
        //delete solution;
        alphas[i][3] = 0;                         //quantidade de execucoes
    }

    int ind = 0;              //variaveis auxiliares a escolha do alpha
    uniform_real_distribution<double> randomFloat(0.0, 1.0);
    default_random_engine re;

    //loop de execucao
    while (time < tRemaining)
    {
        /*
        for(int i = 0; i < alphaRR; ++i)
            cout << alphas[i][1] << " ";
        cout << endl;
         */

        ind = selectAlpha(alphas, alphaRR, randomFloat(re));

        solution = nullptr;
        while (solution == nullptr)
            greedyRandomized(alphas[ind][0]);

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
        cout << time << endl;
    }

    for (int i = 0; i < alphaRR; ++i)
        delete[] alphas[i];
    delete[] alphas;

    solution = bestS;
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



