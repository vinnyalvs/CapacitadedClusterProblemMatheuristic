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
	phase2(&groupList, &nodes, alpha);

	//Fase 3
	phase3(&groupList, &nodes, alpha);

	solution->groupList = groupList;

}

void Heuristic::greedyRandomized2(double alpha){
  	solution = new Solution;
   vector <Node> nodesNotInSol = g->nodes;
   vector <Node> nodesInSol;
   vector <Group> groupList;
   //Seleciono primeiro no
    unsigned int node = rand() % g->nodes.size();
    nodesInSol.push_back(node);
	Group FG;
	FG.weight = 0.0;
	FG.cost = 0.0;
	FG.id = 0;
	FG.nodeList.push_back(node);
	groupList.push_back(FG);
    //vetor de pares com beneficio do no para os nos que estão na solução
    vector <pair<unsigned int, double> > nodeCostRelation;
    vector <pair<unsigned int, double> > minMaxRelation;
	//system("cls");
	//cout << "node: " << node << endl;
    for(auto n1: g->nodes){
        pair <unsigned int, double> p;
        p.first = n1.id;
        p.second = 0 ;
		if (n1.id != node) {
			for (auto n : g->nodes) {
				if (n1.id != n.id)
					p.second += g->edges[n1.id][n.id];

			}
			nodeCostRelation.push_back(p);
		}
    }

    std::sort(nodeCostRelation.begin(), nodeCostRelation.end(), [](pair<unsigned int, double>   lhs, pair<unsigned int, double> rhs)
	{
		return lhs.second > rhs.second;
	});
   // nodeCostRelation.erase(nodeCostRelation.begin() + input->K, nodeCostRelation.end());
    //Inicializo os K grupos com os melhores
    for(int i=1;i< input->K;i++){
        Group G;
        G.weight = 0.0;
        G.cost = 0.0;
		G.id = i;
		int id = rand() % nodeCostRelation.size() * alpha;
		G.insertNode(nodeCostRelation[id].first);
		nodesInSol.push_back(nodeCostRelation[id].first);
		nodeCostRelation.erase(nodeCostRelation.begin() + id);
		groupList.push_back(G);
		
   }
   //Tiro os nos que ja pertencem a um cluster do vetor mais geral de nós
   int i;
   vector <Node> :: iterator it;
   std::sort(nodesInSol.begin(), nodesInSol.end(), []( Node lhs, Node rhs)
   {
	   return lhs.id < rhs.id;
   });
   for(auto r:nodesInSol){
        for (i = 0; i<nodesNotInSol.size(); i++)
			if (nodesNotInSol[i].id == r.id) {

				break;
			}
        nodesNotInSol.erase(nodesNotInSol.begin() + i);
   }

   //Fase 2
    phase2(&groupList,&nodesNotInSol,alpha);

	//Fase 3
	phase3(&groupList,&nodesNotInSol,alpha);

	solution->groupList = groupList;
	//cout << solution->calculateCost() << endl;



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
		while((groupCandidates[group].weight) >=  input->lowerB)
            group = rand() % groupCandidates.size() * alpha;

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

void Heuristic::phase3(vector <Group> *groupList,vector <Node> *nodesNotInSol, double alpha){
    int auxWeightControl = 0;
	int bestGroup = -1;
	unsigned int groupId;
	std::sort(groupList->begin(), groupList->end(), [](Group lhs, Group rhs)
	{
		return lhs.id < rhs.id;
	});
	bool control = false;
	while (!control) {
		double bestGain = 0.0;
		unsigned int nodeId = rand() % nodesNotInSol->size();
		nodeId = nodesNotInSol->at(nodeId).id;
		vector <Group> groupCandidates = *groupList;
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




void Heuristic::shake(){
	vector <Group> groupList = solution->groupList;
	std::sort(groupList.begin(), groupList.end(), [](Group lhs, Group rhs)
	{
		return lhs.id < rhs.id;
	});
	bool control = false;
	double cost = solution->cost;
	int count=0;
	vector <Node> nodes = g->nodes;
	while(!control){
        count++;
        unsigned int node = rand()  % g->nodes.size();
        node =  g->nodes[node].id;
        unsigned int oldGroupId = solution->getGroup(node);
        unsigned int newGroupId = rand() % groupList.size();
        Group ga = groupList[newGroupId];
        Group oldGroup = groupList[oldGroupId];
         if(((ga.weight + g->nodes[node].weight) <= input->upperB) && (oldGroup.weight - g->nodes[node].weight) > input->lowerB) {
            groupList[oldGroupId].removeNode(node);
            groupList[newGroupId].insertNode(node) ;
			solution->groupList = groupList;
			solution->calculateCost();
		} else {
            groupList = solution->groupList;
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
			greedyRandomized2(alphas[i][0]);              //inicializando o somatorio para nao zerar a probabilidade de escolha
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
		clock_t t = clock();
		greedyRandomized2(alphas[ind][0]);
		cout << (clock() - t) / CLOCKS_PER_SEC << endl;
		//shake();
		solution->calculateCost();
		//if(iterationCount % 2 == 0 )
		double cost = solution->cost;
		t = clock();
	//	do {
			cost = solution->cost;
			localSearch(alphas[ind][0]);
			cost = solution->cost;
			trade();
			cout << cost << endl;
			//trade2();
	//	} while (cost < solution->cost);

			cout << (clock() - t) / CLOCKS_PER_SEC << endl;
		if (!solution->isFeasible(input->lowerB, input->upperB)) {
			delete solution;
			deletedCount++;
		} else {
			pSolver->addSolution(solution);
			iterationCount++;
			cost = solution->calculateCost();
			cout << cost << endl;
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
			greedyRandomized2(alphas[i][0]);
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



