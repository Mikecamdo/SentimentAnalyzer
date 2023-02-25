#define CATCH_CONFIG_RUNNER
#include "DSString.h"
#include "catch.hpp"
#include "SentimentAnalyzer.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc == 1) { //catch tests only run if no command line arguments
        Catch::Session().run();
    }

    SentimentAnalyzer mainAnalyzer(argv); //SentimentAnalyzer class where the main functionality of the program occurs
    return mainAnalyzer.analyze(); //returns 0 if program works properly, returns -1 if a file could not be opened
}