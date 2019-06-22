	GRASP{
		construtivo();
		SwapShake();
		BuscaLocal();
		trade();
		trade2();
	}
Lorenza, estou usando o construtivo antigo com a fase 1 antiga, com as modificações sugeridas por você na fase 2 e 3
Construtivo:
	Fase1
		Inicializa K Grupos cada um com respectivo elemento aleatórios
	Fase2
		Sorteia um grupo (candidatos.size * alpha) dos candidatos (candidato aqui é grupo que ainda não atingiu o limite inferior) 
			Para cada nó que ainda não está na solução
				Busca melhor nó a ser inserido
		Insere Melhor nó
		Verifica se grupos atingiram o limite inferior
	Fase3
		Para cada nó que ainda não está na solução
			Busca melhor grupo para inserior nó
		Insere Melhor Nó

O procedimento SwapShake nasceu da percepção que os construtivos geravam soluções de valor médio similar, e de que as buscas locais não davam o salto alto de qualidade que eu precisava
além disso, os artigos levavam mais tempo para gerar 1 solução, então achei que não adiantava uma iteração do GRASP rodar em menos de um segundo se a solução tiver valor baixo.


Solução: SwapShake caminha sorteando elementos de cada grupo e inserindo-os no melhor grupo possível durante uns 5~6 segundos vazendo trocas viáveis entre os grupos,
a troca é realizada sempre que ela for viável, não necessitando de melhorar a solução.

(obs: o artigo também realizava um Shake na solução)

O SwapShake retorna a melhor solução encontrada. O que ocorre durante a iteração é os grupos mais "ricos" acabam ficando com mais nós e os mais "pobres" com menos nós.
Ou seja alguns grupos no limte superior muito cheios e outros no limite inferior.

SwapShake:
	Vetor NósCandidatos
	Solution Best 
	Durante N (Iterações ou segundos)
		Para Cada Grupo
			sorteia um elemento desse grupo, 
			se o grupo poder "doar" o nó (ou seja ele ainda fica acima do limite inferior)
			NósCandidatos.push(nó)
		Para cada candidato
			Insere Melhor Grupo
		Se solução é a melhor até agora
			Best = Solução atual


As buscas locais tem um objetivo de encontrar os melhores benefícios para colocar nos grupos

BuscaLocal:
	Enquanto há melhora na solução
		Para cada nó:
		 examina qual é a melhor "doação" de um grupo para outro
		 guarda a troca (Melhor nó e Melhor grupo)


Trade:
	Enquanto há melhora na solução
		Para cada grupo I
			para cada nó n do grupo I
				para cada outro grupo J != I (de forma que id (J) > (I)) 
					para cada nó m do grupo J
					examina qual é a melhor troca de um grupo para outro
					guarda a troca (Melhor n e m e Melhor grupo I e J)


void Heuristic::greedyRandomized(double alpha)
{
	//Fase 1
	solution = new Solution;
	vector <Node> nodes = g->nodes;
	vector <Group> groupList;
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
	phase2(&groupList, &nodes, alpha);

	//Fase 3
	phase3(&groupList, &nodes, alpha);

	solution->groupList = groupList;

}

void Heuristic::phase2(vector <Group> *groupList,vector <Node> *nodesNotInSol, double alpha){
    bool control = false;
	int count = 0;
	vector <Group> okGroups;
	vector <Group> groupCandidates = *groupList;
	while (!control) {
		std::sort(groupCandidates.begin(), groupCandidates.end(), [](Group lhs, Group rhs)
		{
			return lhs.cost > rhs.cost;
		});
		int bestNode = -1;
		double bestGain = 0.0;
		int group = rand() % groupCandidates.size() * alpha;
		for (int k = 0; k < nodesNotInSol->size(); k++) {
            double gain = calculateGain(&groupCandidates[group], nodesNotInSol->at(k).id);
				if (gain > bestGain) {
					bestNode = nodesNotInSol->at(k).id; //groupCandidates[groupId].id;
					bestGain = gain;
				}
		}
		if (bestNode != -1) {
			groupCandidates[group].insertNode(bestNode);
			int i;
			for (i = 0; i<nodesNotInSol->size(); i++)
				if (nodesNotInSol->at(i).id == bestNode)
					break;
			nodesNotInSol->erase(nodesNotInSol->begin() + i);
		}
		vector <Group>::iterator it;
		for (it = groupCandidates.begin(); it != groupCandidates.end();) {
			if(it->weight >= input->lowerB) {
				okGroups.push_back(*it);
				it = groupCandidates.erase(it);
			}
			else {
				it++;
			}
		}
		if (groupCandidates.size() == 0) {
			for (auto c : okGroups)
				groupCandidates.push_back(c);
			control = true;
		}
			
		if (nodesNotInSol->size() == 0) {
			control = true;
		}
	}
	*groupList = groupCandidates;
}

void Heuristic::phase3(vector <Group> *groupList,vector <Node> *nodesNotInSol, double alpha) {
	int auxWeightControl = 0;
	
	unsigned int groupId;
	std::sort(groupList->begin(), groupList->end(), [](Group lhs, Group rhs)
	{
		return lhs.id < rhs.id;
	});
	bool control = false;
	vector <Group> groupCandidates = *groupList;
	while (!control) {
		double bestGain = 0.0;
		unsigned int nodeId = rand() % nodesNotInSol->size();
		nodeId = nodesNotInSol->at(nodeId).id;
		int bestGroup = -1;
		//ordenar pelo ganho
		for (auto c : groupCandidates) {
			if ((c.weight + g->nodes[nodeId].weight) <= input->upperB) {
				double gain = calculateGain(&c, nodeId);
				if (gain >= bestGain) {
					bestGroup = c.id; //groupCandidates[groupId].id;
					bestGain = gain;
				}
			}
		}
		if (bestGroup != -1) {
			groupCandidates[bestGroup].insertNode(nodeId);
			int i;
			for (i = 0; i<nodesNotInSol->size(); i++)
				if (nodesNotInSol->at(i).id == nodeId)
					break;
			nodesNotInSol->erase(nodesNotInSol->begin() + i);
		}
		if (nodesNotInSol->size() == 0) {
			control = true;
		}
		//	solution->groupList = groupCandidates;
		*groupList = groupCandidates;
		//	cout << "Cost: " << solution->calculateCost() << endl;;
	}

}

void Heuristic::swapShake() {
		time_t tim = (double)((clock()) / CLOCKS_PER_SEC);
		time_t timeStart = clock();
		Solution *n = new Solution();
		n->groupList = solution->groupList;
		Solution *best = new Solution();
		best->groupList = solution->groupList;
		best->cost = solution->cost;
		int count = 0;
		while (tim <= 5)
		{
			count++;
			vector<int> candNovo;
			vector <Group> grupos = n->groupList;
			for (unsigned int j = 0; j<grupos.size(); j++)
			{
				if (grupos[j].nodeList.size() != 0) {
					int k;
					k = rand() % grupos[j].nodeList.size();
					int w = grupos[j].nodeList[k];
					if (grupos[j].weight - g->nodes[w].weight >= input->lowerB) {
						grupos[j].removeNode(w);
						candNovo.push_back(w);
					}
				} 
			}
			for (unsigned int j = 0; j<candNovo.size(); j++)
			{
				BestGV(candNovo[j],&grupos);
			}
			n->groupList = grupos;
			double cost = n->calculateCost();
			if (n->cost > best->cost) {
				best->groupList = n->groupList;
				best->cost = n->cost;
			}
			tim = (double)((clock() - timeStart) / CLOCKS_PER_SEC);
	}
		if (best->cost > solution->cost) {
			solution->groupList = best->groupList;
		}
		cout << solution->calculateCost() << endl;
		//cout << solution->isFeasible(input->lowerB,input->upperB) << endl;
}


void Heuristic::localSearch(double alpha)
{
    Solution *backup = new Solution();
    backup->cost = solution->cost;
    backup->groupList = solution->groupList;
	vector <Node> nodes = g->nodes;
    bool control = false;
    int count =0;
    double cost = backup->cost;
	double deltaCost;
    while(!control){
		Solution *n = new Solution;
		double solCost = cost;
		vector <Group> groupList = backup->groupList;
		unsigned int bestNode=-1, bestGroup;
		double bestNewCost=0;
		for(auto node:nodes){
			int oldGroupId = backup->getGroup(node.id);
			double oldCost = groupList[oldGroupId].cost;
			double oldWeight = groupList[oldGroupId].weight;
			double bestGain=0;
			double newWeight = oldWeight - node.weight;
			if(!(newWeight < input->lowerB)){
				for(auto c: groupList){
					if(((c.weight + node.weight) <= input->upperB) && c.id != oldGroupId){
					   double gain = calculateGain(&c,node.id,0);
					   double dCost = (c.cost + gain);
					   dCost -= oldCost;
					   double newSolCost = solCost + dCost;
					   if (gain > bestGain &&  newSolCost > solCost && newSolCost > bestNewCost) {
						   bestGroup = c.id;
						   bestGain = gain;
						   bestNode = node.id;
						   bestNewCost = newSolCost;
						//   cout << "node: " << bestNode << endl;
					   }
						    
					}
				}
			}
		}
		if (bestNode != -1) {
			int groupId = backup->getGroup(bestNode);
			if (groupId != -1) {
				groupList[groupId].removeNode(bestNode);
				groupList[bestGroup].insertNode(bestNode);
				n->groupList = groupList;
				solCost = n->calculateCost();
			}
		}
		if(solCost <= cost)
			control = true;
		else {
			cost = solCost;
			backup->groupList = n->groupList;
			backup->cost = n->cost;
		}
	}
	//Subsitui a nova solução
    if(solution->cost < backup->cost){
        solution->groupList = backup->groupList;
        solution->cost = backup->cost;
    }
}

void Heuristic::trade(){
    Solution *backup = new Solution();
    backup->cost = solution->cost;
    backup->groupList = solution->groupList;
    vector <Group> groupList = backup->groupList;
	bool control = false;
	double cost = solution->cost;
	int count=0;
	std::sort(groupList.begin(), groupList.end(), [](Group lhs, Group rhs)
	{
		return lhs.id < rhs.id;
	});
	while(!control){
		Solution *n = new Solution;
		unsigned int bestNode = -1, bestNode2 = -1;
		double bestNewCost = 0;
		double solCost = backup->cost;
		for (int i = 0; i < groupList.size(); i++) {
				for (int j = i + 1; j < groupList.size(); j++) {
					int groupA = i, groupB = j;
					Group ga = groupList[groupA];
					Group gb = groupList[groupB];
					for (int k = 0; k < ga.nodeList.size(); k++) {
						unsigned int nodeA = ga.nodeList[k];
						double newWA = ga.weight - g->nodes[nodeA].weight;
						if(newWA > input->lowerB)
							for (int z = 0; z < gb.nodeList.size(); z++) {
								unsigned int nodeB = gb.nodeList[z];
								double newWB = gb.weight - g->nodes[nodeB].weight + g->nodes[nodeA].weight;
								newWA += g->nodes[nodeB].weight;
								if (newWB <= input->upperB && newWB >= input->lowerB && newWA <= input->upperB) {
									double newCA = ga.cost - calculateGain(&ga, nodeA, 0) + calculateGain(&ga, nodeB, 0);
									double newCB = gb.cost - calculateGain(&gb, nodeB, 0) + calculateGain(&gb, nodeA, 0);
									double newSolCost = solCost + (newCA + newCB) - (ga.cost + gb.cost);
									if (newSolCost > solCost && newSolCost > bestNewCost) {
										bestNode = nodeA;
										bestNode2 = nodeB;
										bestNewCost = newSolCost;
								}
							}
					        }
					}
				}
		}
        count++;
		if (bestNode != -1) {
			int groupId = backup->getGroup(bestNode);
			int groupId2 = backup->getGroup(bestNode2);
			if (groupId != -1 &&  groupId2 != -1  ) {
				groupList[groupId].removeNode(bestNode);
				groupList[groupId].insertNode(bestNode2);
				groupList[groupId2].removeNode(bestNode2);
				groupList[groupId2].insertNode(bestNode);
				n->groupList = groupList;
				solCost = n->calculateCost();
			}
		}
		if (solCost <= cost)
			control = true;
		else 
		cost = solCost;
		backup->groupList = n->groupList;
		backup->cost = n->cost;
	}
	//Fim loop, atualiza a sol se há melhora

    if(solution->cost < backup->cost){
        solution->groupList = backup->groupList;
        solution->cost = backup->cost;
    }
}

