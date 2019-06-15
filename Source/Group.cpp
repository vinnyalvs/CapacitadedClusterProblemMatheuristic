//
// Created by edson on 11/10/18.
//

#include "../headers/Group.h"

Graph* Group::g = nullptr;

Group::Group() {

}

Group::Group(unsigned int id)
{
    this->id = id;
    weight = 0.0;
    cost = 0.0;
}

void Group::insertNode(unsigned int index)
{
    weight += g->nodes[index].weight;
    for (auto i : nodeList)
        cost += g->edges[index][i];
    this->nodeList.push_back(index);
}

void Group::insertNode(unsigned int index,int p)
{
    this->nodeList.push_back(index);
}

void Group::removeNode(unsigned int index)
{
    int count=0,aux;
    weight -= g->nodes[index].weight;
    for (auto i : nodeList){
        cost -= g->edges[index][i];
        if(i == index)
            aux = count;
        count++;
    }
    this->nodeList.erase(nodeList.begin() + aux);
}

void Group::removeNode(unsigned int index, int p)
{
    int count=0,aux;
    //weight -= g->nodes[index].weight;
    for (auto i : nodeList){
      //  cost -= g->edges[index][i];
        if(i == index)
            aux = count;
        count++;
    }
    this->nodeList.erase(nodeList.begin() + aux);
}


void Group::printGroup()
{
    cout << "{";
    for (auto i : nodeList)
    {
        cout << i;
        if (i != nodeList.back())
            cout << ", ";
    }
    cout << "}";
}
