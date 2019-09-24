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

void Input::readSolution(string path, vector <vector<int>> *clusters) {
	ifstream reader;
	reader.open(path);
	string aux = " ";
	int K = 8;
	unsigned int q;
	for (int i = 0; i < K; i++) {
		vector <int> a;
		clusters->push_back(a);
	}
	int count = 0;
	cout << path << endl;
	if (reader.is_open())
	{

		while (count < K) {
			while (aux != ";") {
				reader >> aux;
				stringstream geek(aux);
				 //cout << aux << endl;
				if (aux == ";")
					break;
				int x;
				geek >> x;
				cout << x << endl;
				clusters->at(count).push_back(x);
			}
			cout << count << endl;
			aux = " ";
			count++;
		}

	}
	else
		cout << "Falha na leitura da Solução!" << endl; 
	reader.close();
}
	

void Input::readFile(string path, vector <vector<int>> *clusters) {

	ifstream inFile;
	inFile.open(path);

	// pega tamanho do arquivo, para ler e salvar tudo no buffer
	inFile.seekg(0, std::ifstream::end);
	auto length = static_cast<unsigned int>(inFile.tellg());
	inFile.seekg(0, std::ifstream::beg);
	//criando e inicializando buffer
	//buffer contem o arquivo inteiro
	auto *buffer = new char[length];
	inFile.read(buffer, length);
	inFile.close();

	//Divide em linhas
	char *p = strtok(buffer, "\n");
	//Pega proxima linha
	//strtok(nullptr, "\n");
	//p = strtok(nullptr, "\n");
	//Pega valor da sulucao que foi salvo
	string valorSolucao = p;


	stringstream stream;
	string strAux;
	int vertAux;
	vector<vector<int>> grupos;

	// Pega linha da solução
	p = strtok(nullptr, "\n");
	// Reparte em grupos
	char *p2 = strtok(p, "}");
	while (p2)
	{
		vector<int> matLine;
		for (int i = 0; p2[i] != '\0' || !strAux.empty(); ++i)
		{
			if (p2[i] >= '0' && p2[i] <= '9')
				strAux.push_back(p2[i]);
			else if (p2[i] == ',' || p2[i] == '}' || p2[i] == '\0')
			{
				if (!strAux.empty())
				{
					vertAux = stoi(strAux);
					matLine.push_back(vertAux);

				}
				if (p2[i] == '\0')
					--i;
				strAux.clear();
			}
		}
		if (!matLine.empty())
			grupos.push_back(matLine);
		p2 = strtok(nullptr, "}");
	}

	*clusters = grupos;
	/*
	float soma = 0;
	vector<float> somaCustoGrupos(grupos.size(), 0);
	int contaVertices = 0;
	set<int> uniqueElements;
	for (int i = 0; i < grupos.size(); ++i)
	{
		for (int j = 0; j < grupos[i].size(); ++j)
		{
			cout << grupos[i][j] << endl;
			/*contaVertices++;
			somaCustoGrupos[i] += g->edges[i][j];
			uniqueElements.insert(grupos[i][j]);
			for (int k = j; k < grupos[i].size(); ++k)
			{
				int vert1 = grupos[i][j];
				int vert2 = grupos[i][k];
				soma += g.matrizDistancia[vert1][vert2];
			} 
		}
	} */

	/*cout << "Instancia: " << instanceName << endl;
	cout << "Beneficio informado: " << valorSolucao << endl;
	cout << "Beneficio calculado: " << soma << endl;
	cout << "Numero de Vertices: " << contaVertices << endl;
	cout << "Numero de elementos unicos: " << uniqueElements.size() << endl;
	cout << "Numero de Grupos: " << grupos.size() << endl;
	cout << "Custo dos Grupos:" << endl;
	for (int i = 0; i < somaCustoGrupos.size(); ++i)
	{
		cout << "Grupo " << i << ": " << somaCustoGrupos[i] << endl;
	}
	bool inviavel = false;
	float upLimit = graph.getUpperLimit();
	float lowLimit = graph.getLowerLimit();
	for (int i = 0; i < somaCustoGrupos.size(); ++i)
	{
		if (somaCustoGrupos[i] < lowLimit || somaCustoGrupos[i] > upLimit)
		{
			cout << "Inviavel no grupo " << i << "." << endl;
			inviavel = true;
		}
	}
	if (inviavel)
		cout << "SOLUCAO INVIAVEL!!!!!!!!!!!" << endl;
	else
		cout << "SOLUCAO VIAVEL!!!!!!!!!!!" << endl; */

}


void Input::readFile2(string path, vector <vector <vector<int>>> *clustersList) {

	ifstream inFile;
	inFile.open(path);
	string line;
 
	int numSol = 10;
	
		int countLine = 4;
		while (getline(inFile, line)) {
		vector<vector<int>> grupos;
			if (countLine % 3 == 1) {
			//	std::cout << "asidjkas" << line << "n";
			}
			else if (countLine % 3 == 2) {
				cout << countLine;
			//	std::cout << line << "n";
				char *strAux2 = new char[line.length() + 1];
				strcpy(strAux2, line.c_str());
			//	char *p = strtok(strAux2, "\n");
			//	cout << *p << endl;
			//	string valorSolucao = p;

				stringstream stream;
				string strAux;
				int vertAux;

				// Pega linha da solução
				char *p = strtok(strAux2, "\n");
				// Reparte em grupos
				char *p2 = strtok(p, "}");
				while (p2)
				{
					vector<int> matLine;
					for (int i = 0; p2[i] != '\0' || !strAux.empty(); ++i)
					{
						if (p2[i] >= '0' && p2[i] <= '9')
							strAux.push_back(p2[i]);
						else if (p2[i] == ',' || p2[i] == '}' || p2[i] == '\0')
						{
							if (!strAux.empty())
							{
								vertAux = stoi(strAux);
								matLine.push_back(vertAux);

							}
							if (p2[i] == '\0')
								--i;
							strAux.clear();
						}
					}
					if (!matLine.empty())
						grupos.push_back(matLine);
					p2 = strtok(nullptr, "}");
				}
				clustersList->push_back(grupos);
			}				
			countLine++;
	}
	inFile.close();

}