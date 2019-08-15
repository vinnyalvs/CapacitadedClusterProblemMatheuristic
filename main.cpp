#include <iostream>
#include "headers/Input.h"
#include "headers/Heuristic.h"
#include "headers/Group.h"
#include "headers/Solution.h"
#include <time.h>
#include <iomanip>
#include <string>

using namespace std;



int main(int arc, char **argv)
{
  srand(13);
   string instanceList = "../CCP/Instancias/instanceList.txt";
   string outputPath = "../CPP/output.txt";
   string path = "C:/Users/HAL/Documents/GitHub/TCCExatas/Sol.txt";
   Input input;
   input.readInstance(argv[1], 1);
 //input.readSolution(path);
   Heuristic h(&input);
  //h.greedyRandomized2(0.1);
		
   h.greedyRandomizedReactive(10, 1, 500, 13);
   h.runSolver();
   cout << "--------------" << endl; 

    return 0;
}



