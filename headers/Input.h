//
// Created by edson on 09/10/18.
//

#ifndef TRABALHO_IC_DEFINICAO_GRUPOS_INPUT_H
#define TRABALHO_IC_DEFINICAO_GRUPOS_INPUT_H

#include "Graph.h"
#include "Group.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream> 
#include <set>
class Input {

public:
    ~Input();
    unsigned int K; //number of groups
    double lowerB;
    double upperB;
    Graph *g;
	void readFile(string path, vector <vector<int>> *clusters);
	void readFile2(string path, vector <vector <vector<int>>> *clustersList);
    void readInstance(string path, int type);
	void readSolution(string path, vector <vector<int>> *clusters);
private:
    void readType1(string path);
    void readType2(string path);

};


#endif //TRABALHO_IC_DEFINICAO_GRUPOS_INPUT_H
