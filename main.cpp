#include <iostream>
#include "headers/Input.h"
#include "headers/Heuristic.h"
#include "headers/Group.h"
#include "headers/Solution.h"
#include <time.h>
#include <iomanip>
#include <string>

using namespace std;

void run(const string &instanceList, string outputPath);
void verifySolution(string path,string originalPath,int type);

int main(int arc, char **argv)
{
   string instanceList = "../CCP/Instancias/instanceList.txt";
   string outputPath = "../CPP/output.txt";
   Input input;
   //DCC136-82_02.txt"

    input.readInstance(argv[1], 1);

    clock_t endT,initialT = clock(),elapsedT;
   Heuristic h(&input);
    endT = clock();
    elapsedT = ((endT - initialT) / (CLOCKS_PER_SEC)); // Converte tempo gasto em segundos
   // double tRemaining = 60 - (double) elapsedT;
    h.greedyRandomizedReactive(10, 1, 10, 13);
    h.runSolver();
   // h.solution->printSolution();
   // cout << "cost: " << h.solution->calculateCost() << " "<< h.solution->isFeasible(input.lowerB, input.upperB) << endl;
   // run(instanceList,outputPath);
   // verifySolution("../DefinicaoClusters/verify.txt","../DefinicaoClusters/instancias/TipoA/DCC136-82_03.txt",1);
    return 0;
}

void run(const string &instanceList, string outputPath) {
    unsigned long seed[10] = {13, 29, 47, 59, 83, 101, 127, 167, 181, 199};

    clock_t initialT, endT , elapsedT;
    Input i;

   //string path = "../DefinicaoClusters/instancias/TipoA/DCC136-82_01.txt";
    string path;

    ifstream instanceReader;
    ofstream outputWriter;
    double solutionCost, timeEnd, sAuxCost, timeAuxEnd;
    Solution *s;

    double bestCost=0.0;
    int numIndependentExecutions = 1;

    instanceReader.open(instanceList);
    outputWriter.open(outputPath);
    double improvementPercentual =0;

    int count=0;
    int type=1;
    while (instanceReader >> path) {
        count++;
        if (count > 15) // Gambiarra para ler as 15 instancias do tipo1 e depois ler as do tipo 2
            type = 2;
        cout << "EXECUCAO DA INSTANCIA: " << path << endl;

            initialT = clock();
            i.readInstance(path + ".txt", type);
            /*calcula aproximadamente o tempo em milisegundos gasto entre as duas chamadas de clock()*/
            endT = clock();

            elapsedT = ((endT - initialT) / (CLOCKS_PER_SEC)); // Converte tempo gasto em segundos
            double tRemaining = 60 - (double) elapsedT; //Tem o tempo remanescente em segundos
            Heuristic h(&i);
            h.greedyRandomizedReactive(10, 1, tRemaining,seed[1]);
            //h.greedyRandomized(0.4);
           //h.localSearch2(60 - tRemaining*0.9);
            s = h.solution;
            endT = clock();
            elapsedT = ((endT - initialT) / (CLOCKS_PER_SEC / 1000));
            solutionCost = s->calculateCost();
            cout << solutionCost << endl;
            cout << s->isFeasible(i.lowerB,i.upperB) << endl;
             s->printSolution();
            improvementPercentual = improvementPercentual/solutionCost;


            cout << s->isFeasible(i.lowerB,i.upperB) << endl;
        outputWriter << "Solucao  " << path << "  Custo: ";
        outputWriter << setprecision(8) << solutionCost << "Tempo" << elapsedT << "Melhora" << improvementPercentual << ";\n";
        outputWriter << "S=";
        outputWriter << " { ";
        for (auto i : s->groupList) {
            //------------ Print Group
            outputWriter << " { ";
            for (auto j : i.nodeList) {
                outputWriter << j;
                if (j != i.nodeList.back())
                    outputWriter << " , ";
            }
            outputWriter << " } ";
            //------------
            if (i.nodeList.front() != s->groupList.back().nodeList.front())
                outputWriter << " , " << endl;
        }
        outputWriter << "}\n";


        cout << setprecision(3) << fixed << "execucao " << count << " | custo C1 " << solutionCost
             << " | tempo C1 " << elapsedT << " | tempo C2 "
             << " | seed " << seed[1] << " | " << endl;

        delete s;
    }

    outputWriter.close();
    instanceReader.close();
}



void verifySolution(string path, string originalPath, int type) {

    Input input;
    input.readInstance(originalPath,type);
    ifstream file;
    file.open(path);

    int *ids = new int [input.g->order];

    string aux;
    char aux2;
     int nodeId;
    vector <Group> groupList;
    unsigned int count=0;
    Group *group;
    group = new Group(count);
    if(file.is_open()){
        file >> aux;
        file >> aux;
        while(file >> aux) {
            cout << aux << endl;
            if( aux == "{" || aux == "," ){
                cout << "Do nothing" << endl;
            } else if(aux == "},"  || aux == "}") {
                file >> aux;
                cout << aux << endl;
                count++;
                groupList.push_back(*group);
                group = new Group(count);

            }  else {
                nodeId =  stoul(aux);
                ids[nodeId] = nodeId;
                group->weight += input.g->nodes[nodeId].weight;
                group->nodeList.push_back(nodeId);
                }
        }



        double sum = 0.0;
        //Para cada grupo
        for (auto i : groupList)
        {


            for(int j = 0; j<i.nodeList.size()-1; j++)
            {

                for(int k = j+1; k<i.nodeList.size(); k++){
                    sum += Group::g->edges[i.nodeList[j]][i.nodeList[k]];
                }
            }



        }

        cout << sum << endl; // Calculo da função objetivo

        // Confere o peso dos grupos
        for (auto i : groupList)
        {
            if(i.weight < input.lowerB  ||  i.weight > input.upperB ){
                cout << i.weight << endl;
            }


        }


        //Confere se não tem dois nós repetidos ( Ou seja o mesmo nó em grupos diferentes )
        for (int i = 0; i < input.g->order ; i++)
        {
            cout << ids[i] << endl;
            if (ids[abs(ids[i])] >= 0)
                ids[abs(ids[i])] = -ids[abs(ids[i])];
            else
               cout << "Falso" << endl;
        }

        cout << "Valido" << endl;

        delete group;
        delete ids;

    } else {
        cout << "Falha ao abrir arquivo" << endl;
    }



}
