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
   input.readInstance(argv[1], 1);
   Heuristic h(&input);
  // h.greedyRandomized2(0.1);
   h.greedyRandomizedReactive(10, 1, 100, 13);
   //h.runSolver();
   cout << "--------------" << endl;

    return 0;
}



