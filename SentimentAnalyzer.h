//
// Created by mikec_g1kgiu8 on 9/6/2021.
//

#ifndef MAIN_CPP_SENTIMENTANALYZER_H
#define MAIN_CPP_SENTIMENTANALYZER_H
#include "DSString.h"
#include <vector>

using namespace std;

class SentimentAnalyzer {
    private:
        char** argv; //holds the command line arguments

        void addPositiveWord(vector<DSString>&, vector<int>&, const DSString&); //these functions are private as they do not need to be accessed in main
        void addNegativeWord(vector<DSString>&, vector<int>&, const DSString&);
        int findWord(vector<DSString>, vector<int>, const DSString&);

    public:
        SentimentAnalyzer();
        SentimentAnalyzer(char**);
        ~SentimentAnalyzer();

        int analyze(); //function for the main functionality of the program
};


#endif //MAIN_CPP_SENTIMENTANALYZER_H
