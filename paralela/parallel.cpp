/*
 * Os comentários em Inglês são originais do wildcard-pattern-matching
 * Os comentários em Português são das nossas alterações.
 * source: https://www.geeksforgeeks.org/wildcard-pattern-matching/
*/

// C++ program to implement wildcard
// pattern matching algorithm
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <string>
#include <omp.h>
#include <time.h>

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

int main()
{
    using namespace std::chrono;

    //Inicia a contagem de tempo de execução
    auto start = high_resolution_clock::now();

    int th_id, nthreads;

    vector<string> patternFile_contents = getAllLines("pattern.txt");
    vector<string> textFile_contents = getAllLines("text.txt");
    results.open("results.txt");

    //Variáveis para auxiliar a contagem de padrões encontrados
    int counter = 0;
    string line;
    int i = 0;
    int j = 0;

    //Trecho responsável por realizar a contagem de padrões paralelamente
    #pragma omp parallel for private(i, line) reduction(+:counter) num_threads(4) collapse(2)
    for(i=0; i < textFile_contents.size(); i++)
        for(j=0; j < patternFile_contents.size(); j++){
            line = textFile_contents[i];
            string pattern = patternFile_contents[j];
            counter += strmatch(line, pattern, line.length(), pattern.length());
        }

    std::cout << "Number of matches: " << counter << endl;
    results << "Number of matches: " << counter << endl;

    // Finaliza a contagem de tempo de execução
    auto stop = high_resolution_clock::now();
    // Obtém o tempo de excução em microsegundos
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;
    results << "Time taken by function: "
         << duration.count() << " microseconds" << endl;

    results.close();

    return 0;
}
