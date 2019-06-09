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