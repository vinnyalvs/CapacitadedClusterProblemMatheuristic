//
// Created by edson on 09/10/18.
//

#include "../headers/Node.h"

Node::Node(unsigned int id)
{
    this->id = id;
    degree = 0;
}

bool Node::edgeSorter(Edge e1, Edge e2)
{
    return e1.second > e2.second;
}