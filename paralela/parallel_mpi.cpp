#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int soma;
int soma_final;
int chunksize;

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

//Função responsável por extrair todas as linhas de um arquivo
vector<string> getAllLines(string nameOfFile){
    string str;
    ifstream myFile(nameOfFile, std::ifstream::in);
    vector<string> linesOfFile;
    while (getline(myFile, str))
    {
        linesOfFile.push_back(str);
    }
    myFile.close();
    return linesOfFile;
}

int main(int argc, char **argv) {

	int size, rank;
	int val;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// MASTER
	if(rank == 0) {
		// Preencher vetor
		vector<string> patternFile_contents = getAllLines("pattern.txt");
    	vector<string> textFile_contents = getAllLines("text.txt");
    	results.open("results.txt");

		chunksize = textFile_contents.size()/size;

		// Envia chunk para cada processo
		for(int i=1; i<size; i++) {
			int begin = i*chunksize;
			MPI_Send(&textFile_contents[begin], chunksize, MPI_, i, 100, MPI_COMM_WORLD);
		}

		// Mestre processa chunk local
		// Verifica se valor eh primo
		for(int i=0; i<chunksize; i++) {
			resultado[i] = ehprimo(vetor[i]);
		}

		// Recebe respostas dos outros processos
		for(int i=1; i<size; i++) {
			MPI_Recv(auxbuf, chunksize, MPI_INT,
			MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            int source = status.MPI_SOURCE;
			int begin = source*chunksize;
            memcpy(&resultado[begin], auxbuf, chunksize*sizeof(int));
		}
	}else {
		// Recebe chunk
		MPI_Recv(vetor, chunksize, MPI_INT, 0,
		MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		// Processa chunk
		for(int i=0; i<chunksize; i++) {
			resultado[i] = ehprimo(vetor[i]);
		}

		// Envia respostas
		MPI_Send(resultado, chunksize, MPI_INT, 0,
		100, MPI_COMM_WORLD);
	}

	// if(rank == 0) {
	// 	// Dump resultado
	// 	for(int i=0; i<VETSIZE; i++) {
	// 		printf("%d", resultado[i]);
	// 	}
	// 	printf("\n");
	// }

	MPI_Finalize();
	return 0;
}