
//
// Created by edson on 11/10/18.
//


#include <fstream>

#include "../headers/Solution.h"

Solution::Solution()
{
}

Solution::~Solution()
{
}

void Solution::printSolution()
{
    cout << "{";
    for (auto i : groupList)
    {
        i.printGroup();
        cout << " weight: " << i.weight;
        if (i.nodeList.front() != groupList.back().nodeList.front())
            cout << ", " << endl;
    }
    cout << "}\n";
    cout << endl;
}



double Solution::calculateCost()
{
    double sum = 0.0;
    //Para cada grupo
    for (auto i : groupList)
    {
        sum += i.cost;
    }

    cost = sum;
    return sum;
}

      // cout << "Grupo "   << endl;
        //------------ Print Group
        //Para cada grupo
            //Para cada nó do grupo
                //Calcula soma das arestas dele para cada outro nó do grupo ( se a aresta existir)
         // retorna soma

       /* for(int j = 0; j<i.nodeList.size()-1; j++)
        {
          //  cout << "No: " << i.nodeList[j] << endl;
            for(int k = j+1; k<i.nodeList.size(); k++){
               // cout << "Noo : " << i.nodeList[k] << endl;
                //if(Group::g->edges[i.nodeList[j]][i.nodeList[k]] >= 0 ){
                    sum += Group::g->edges[i.nodeList[j]][i.nodeList[k]];
                    //cout << "Valor " << Group::g->edges[i.nodeList[j]][i.nodeList[k]] << endl;
                //}



        } */

int Solution::getGroup(unsigned int nodeId){
    for(auto c: groupList){
        for(auto n: c.nodeList )
            if(nodeId == n)
                return c.id;
    }
    return -1;
}

bool Solution::isFeasible(double lowerB,double upperB){
    //Checa se algum grupo não ucmpre as restrições
    int *ids = new int [Group::g->order];

    vector<int> list;
    int count =0;


    for (auto i : groupList)
    {
        for(auto j : i.nodeList ){
            list.push_back(j);
            if( j<0 || j>Group::g->order){
                cout << "id de no invalido" << endl;
                return false;
            }

        }
        if(i.weight < lowerB  ||  i.weight > upperB ){
            cout << "grupo fora dos limites" << endl;
            return false;
        }

        count++;
    }
    //Checa se ainda há algum nó disponpivel ( não pertence a nenhum grupo)

    for(auto n: list ){
        if(getGroup(n) == -1){
            cout << "No sem grupo" << endl;
            return false;
        }

    }

   // cout << list.size() << endl;
    int i=0;
	sort(list.begin(), list.end());
    for(auto c: list) {

		if (list[abs(list[i])] >= 0) {

			list[abs(list[i])] = -list[abs(list[i])];

		}
		else {

            cout << "nos repetidos" << endl;
            return false;
		}
        i++;
    }




    //Se passou pelas duas validações a sol é válida

    return true;
}



