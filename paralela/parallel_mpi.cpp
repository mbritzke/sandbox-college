#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <string>
#include <limits>

using namespace std;

//Variáveis para obter os 3 arquivos
ofstream patternFile; // Arquivo de padrões
ofstream textFile;    // Arquivo de texto
ofstream results;     // Arquivo do resultado da execução do programa

// Function that matches input str with given wildcard pattern
int strmatch(string str, string pattern, int n, int m)
{
    int matchCounter = 0;
    // empty pattern can only match with empty string
    if (m == 0)
        return (n == 0);

    // lookup table for storing results of subproblems
    bool lookup[n + 1][m + 1];

    // initailze lookup table to false
    memset(lookup, false, sizeof(lookup));

    // empty pattern can match with empty string
    lookup[0][0] = true;

    // Only '*' can match with empty string
    for (int j = 1; j <= m; j++)
        if (pattern[j - 1] == '*')
            lookup[0][j] = lookup[0][j - 1];

    int j;
    // fill the table in bottom-up fashion
    for (int i = 1; i <= n; i++)
    {
        for (j = 1; j <= m; j++)
        {
            // Two cases if we see a '*'
            // a) We ignore ‘*’ character and move
            //    to next  character in the pattern,
            //     i.e., ‘*’ indicates an empty sequence.
            // b) '*' character matches with ith
            //     character in input
            if (pattern[j - 1] == '*')
                lookup[i][j] = lookup[i][j - 1] ||
                               lookup[i - 1][j];

            // Current characters are considered as
            // matching in two cases
            // (a) current character of pattern is '?'
            // (b) characters actually match
            else if (pattern[j - 1] == '?' ||
                    str[i - 1] == pattern[j - 1]){
                lookup[i][j] = lookup[i - 1][j - 1];
	    }

            // If characters don't match
            else lookup[i][j] = false;
        }
    }
    //Incrementa o contador de padrões, caso a tabela na posição n e m seja true
    if(lookup[n][m])
        matchCounter++;

    return matchCounter;
}

std::fstream& GotoLine(std::fstream& file, unsigned int num){
    file.seekg(std::ios::beg);
    for(int i=0; i < num; ++i){
        file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
    return file;
}

//Função responsável por extrair todas as linhas de um arquivo
vector<string> getAllLines(string nameOfFile, int from, int to){
    string str;
    fstream myFile(nameOfFile);
    vector<string> linesOfFile;
    if(to == 0){
       while (getline(myFile, str))
       {
	   linesOfFile.push_back(str);
       }
    } else{
       GotoLine(myFile, from);
       for(int i = from; i < to; i++){
          getline(myFile, str);
          linesOfFile.push_back(str);
       }
    }
    myFile.close();
    return linesOfFile;
}

int main(int argc, char **argv) {

	int size, rank;
	int val;
	MPI_Status status;

    int soma;
    int soma_final;
    int chunksize;
    int tamanhoVetorPadroes;
    int inicio, fim;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// MASTER
	if(rank == 0) {
    double t1,t2;
    t1 = MPI_Wtime();  // inicia a contagem do tempo
		// Preencher vetor
	  vector<string> patternFile_contents = getAllLines("pattern.txt", 0, 0);
    vector<string> textFile_contents = getAllLines("text.txt",0, 0);
  	results.open("results.txt");

		chunksize = textFile_contents.size()/size;
    tamanhoVetorPadroes = patternFile_contents.size();

		// Envia chunk para cada processo
		for(int i=1; i < size; i++) {
			int begin = i*chunksize;
      MPI_Send(&begin, 1, MPI_INT, i, 100, MPI_COMM_WORLD);
      int to = begin + chunksize;
      MPI_Send(&to, 1, MPI_INT, i, 200, MPI_COMM_WORLD);
		}

    soma_final = 0;
    for(int i = 0; i < chunksize; i++){
        for(int j=0; j < tamanhoVetorPadroes; j++) {
          soma_final += strmatch(textFile_contents[i], patternFile_contents[j], textFile_contents[i].length(), patternFile_contents[j].length());
        }
    }

		// Recebe respostas dos outros processos
		for(int i=1; i<size; i++) {
			MPI_Recv(&soma, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      int source = status.MPI_SOURCE;
      soma_final += soma;
		}
    t2 = MPI_Wtime();
    cout << "Resultado: " << soma_final << endl;
    cout << "Tempo: " << t2-t1 << endl;
	}else {
    vector<string> padroes = getAllLines("pattern.txt", 0, 0);

    MPI_Recv(&inicio, 1, MPI_INT, 0, 100, MPI_COMM_WORLD, &status);
    MPI_Recv(&fim, 1, MPI_INT, 0, 200, MPI_COMM_WORLD, &status);
    //cout << "Inicio: " << inicio << "  fim: " << fim << endl;
    vector<string> linhas = getAllLines("text.txt", inicio, fim);
    //cout << "tamanho: " << linhas.size() << endl;
    soma = 0;
    for(string linha : linhas){
      for(string padrao : padroes){
        soma += strmatch(linha, padrao, linha.length(), padrao.length());
      }
    }
		// Envia resposta
		MPI_Send(&soma, 1, MPI_INT, 0, 100, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}

