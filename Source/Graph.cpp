//
// Created by edson on 09/10/18.
//

#include "../headers/Graph.h"

Graph::Graph(unsigned int order)
{
    this->order = order;
    edges = new double* [order];
    for (unsigned int i = 0; i < order; ++i)
    {
        nodes.emplace_back(i);
        edges[i] = new double[order];
    }
}

Graph::~Graph()
{
    for (int i = 0; i < order; ++i)
        delete[] edges[i];
    delete[] edges;
}

void Graph::printNodeEdges(unsigned int id)
{
    for (auto i : nodes[id].edges)
        cout << i.first << " " << i.second << endl;
}