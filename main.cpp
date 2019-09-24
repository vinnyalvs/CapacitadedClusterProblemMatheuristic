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
   string path = "C:/Users/vinic/Documents/GitHub/CCP/Files/RespostasSparse82/Sparse82_01.txt";
   string path2 = "C:/Users/vinic/Documents/GitHub/CCP/Files/RespostasSparse82/resposta.txt";
   Input input;
   input.readInstance(argv[1], 1);
   vector <vector <vector<int>>> clustersList;
   Heuristic h(&input);
   input.readFile2(path, &clustersList);
	 for (int i = 0; i < 5000; i++) {
	   h.buildSolFromFile(clustersList[i]);
   }
	 h.runSolver();
  // h.greedyRandomizedReactive(10, 1, 130, 13);
   
 /*  for (int i = 0; i < 5000; i++) {
	   vector <vector<int>> clusters1;
	   input.readSolution(path, &clusters1);
	   h.buildSolFromFile(clusters1);
	//   h.construtivo();
	 //  cout << i << endl;
	
   } */
  // h.runSolver();
  /* input.readSolution(path2, &clusters2);
   input.readSolution(path3, &clusters3); 
   input.readSolution(path7, &clusters4);*/

  
//	h.buildSolFromFile(clusters2);
  // h.buildSolFromFile(clusters3); 
   //h.buildSolFromFile(clusters4);
  // h.runSolver();  
  // Heuristic h(&input);
  // h.greedyRandomizedReactive(10, 1, 130, 13);

   cout << "--------------" << endl; 

    return 0;
}


