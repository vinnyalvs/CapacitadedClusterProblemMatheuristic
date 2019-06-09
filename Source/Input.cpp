//
// Created by edson on 09/10/18.
//

#include "../headers/Input.h"

Input::~Input()
{
    delete g;
}

void Input::readInstance(string path, int type)
{
    if (type == 1 || type == 2)
    {
        if (type == 1)
            readType1(path);
        else
            readType2(path);
        Group::g = this->g;
    }

}

void Input::readType1(string path)
{

    ifstream reader;
    reader.open(path);
    string aux;
    unsigned int q;
    if (reader.is_open())
    {
        reader >> q;
        g = new Graph(q);
        reader >> K;
		reader >> aux;
        reader >> lowerB;
        reader >> upperB;
		

        //descartando os outros limites (pois sao todos iguais)
        for(int i = 0;i < K-1; ++i)
            reader >> aux >> aux;
        reader >> aux; //descartando W

        //lendo pesos dos nos
        for (int i = 0; i < g->order; ++i)
            reader >> g->nodes[i].weight;

        unsigned int id1, id2;
        double weight;
        //lendo as arestas
        while (reader >> id1 >> id2 >> weight)
        {
            g->edges[id1][id2] = weight;
            g->edges[id2][id1] = weight;

            if (weight != 0.0)
            {
                g->nodes[id1].edges.emplace_back(make_pair(id2, weight));
                g->nodes[id2].edges.emplace_back(make_pair(id1, weight));
                ++g->nodes[id1].degree;
                ++g->nodes[id2].degree;
            }
        }
    }
    else
        cout << "Falha na leitura da instancia!" << endl;
}

void Input::readType2(string path)
{
    ifstream reader;
    reader.open(path);
    string aux;
    unsigned int q;
    if (reader.is_open())
    {
        reader >> q;
        g = new Graph(q);
        reader >> K;
        reader >> upperB;
        lowerB = 0.0;

        //lendo pesos dos nos
        for (int i = 0; i < g->order; ++i)
            reader >> g->nodes[i].weight;

        //lendo as arestas
        for (int i = 0; i < g->order; ++i)
            for (int j = 0; j < g->order; ++j)
            {
                reader >> g->edges[i][j];
                g->edges[j][i] = g->edges[i][j];
                if (g->edges[i][j] > 0)
                    g->nodes[i].edges.emplace_back(make_pair(j, g->edges[i][j]));
            }

    }
    else
        cout << "Falha na leitura da instancia!" << endl;
}

/*  struct changeGroup c;
            c.groupId = i.id;
            k = 0;
            oldWeight = i.weight;
            for(auto j: i.nodeList ){
               double newWeight = i.weight + g->nodes[nodeId].weight  - g->nodes[j].weight;
               double newWeight2 = newSol->groupList[group1].weight - g->nodes[nodeId].weight + g->nodes[j].weight;
                if (newWeight <= input->upperB  && (newWeight > input->lowerB ) && oldWeight < newWeight ) {
                    canEnterG1 = true;
                }
                if ( newWeight2 <= input->upperB &&  newWeight2 > input->lowerB && newSol->groupList[group1].weight < newWeight ) {
                    canEnterG2 = true;
                }
                if(canEnterG1 && canEnterG2){
                    c.nodeId = j;
                    c.removedPos = k;
                    cg.push_back(c);
                    c.newWeight = newWeight;
                    c.newWeight2 = newWeight2;
               }
                k++;
            }

            for(auto c : cg){
                double newWeight;

            } */