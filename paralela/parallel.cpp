//source: https://www.geeksforgeeks.org/wildcard-pattern-matching/

// C++ program to implement wildcard
// pattern matching algorithm
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <string>
#include <omp.h> 

using namespace std;

ofstream patternFile;
ofstream textFile;

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
                    str[i - 1] == pattern[j - 1])
                lookup[i][j] = lookup[i - 1][j - 1];

            // If characters don't match
            else lookup[i][j] = false;
        }
        if(str[i-1] == ' ' && lookup[i-2][j-2])
            matchCounter++;
    }
    if(lookup[n][m])
        matchCounter++;
    return matchCounter;
    //return lookup[n][m];
}

vector<string> getAllLines(string nameOfFile){
    string str;
    ifstream myFile(nameOfFile, std::ifstream::in);
    vector<string> linesOfFile;
    while (getline(myFile, str))
    {
        linesOfFile.push_back(str);
        //linesOfFile.push_back('\n');
    }

    myFile.close();
    return linesOfFile;
}

void closeFiles()
{
    patternFile.close();
    textFile.close();
}

int main()
{
    int th_id, nthreads;

    vector<string> patternFile_contents = getAllLines("pattern.txt");
    vector<string> textFile_contents = getAllLines("text.txt");

    int counter = 0;
    for(string line : textFile_contents){
        
        #pragma omp parallel private(th_id, nthreads) num_threads(4)
        {
            th_id = omp_get_thread_num();
            nthreads = omp_get_num_threads();
            for(string pattern : patternFile_contents){
                counter += strmatch(line, pattern, line.length(), pattern.length());
            }
            printf("Hello World from thread %d of %d threads.\n", th_id, nthreads);
        }
    }

    std::cout << "Number of matches: " << counter << endl;

    closeFiles();

    return 0;

}
