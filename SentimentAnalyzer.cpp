//
// Created by mikec_g1kgiu8 on 9/6/2021.
//

#include "SentimentAnalyzer.h"
#include <fstream>
#include <algorithm>
#include <iomanip>

using namespace std;

SentimentAnalyzer::SentimentAnalyzer() { //default constructor
    argv = nullptr;
}

SentimentAnalyzer::SentimentAnalyzer(char** files) {
    argv = files;
}

SentimentAnalyzer::~SentimentAnalyzer() = default; //default destructor

int SentimentAnalyzer::analyze() {
    vector<DSString> allWords; //vector that will hold the words from the training file
    vector<int> allWordsCount; //vector that will hold the values of each word from the training file

    ifstream training_data(argv[1]);

    if (!training_data.is_open()) {
        cout << "Could not open training file." << endl;
        return -1;
    }

    int sentiment;
    DSString message; //DSString that will temporarily hold each tweet
    DSString word; //DSString for each individual word in the tweets
    char buffer[400]; //char array to read in the important data
    char dummy[100]; //char array to discard the useless information

    training_data.getline(dummy, 50);
    training_data >> sentiment;

    while (!training_data.eof()) {
        for(int i = 0; i < 5; i++) { //loop that skips the useless information
            training_data.getline(dummy, 100, ',');
        }

        training_data.getline(buffer, 400); //reads in the tweet
        message = buffer;


        int numSpaces = message.count(' '); //counts the number of spaces in each tweet

        if (sentiment == 4) { //for positive messages
            word = message.substring(0, message.find(' ')); //finds the first word

            addPositiveWord(allWords, allWordsCount, word); //adds word to list with a positive sentiment

            for (int i = 1; i < numSpaces; i++) { //loop that finds the middle words
                word = message.substring(message.find(' ', i) + 1, (message.find(' ', i + 1) - message.find(' ', i)) - 1);

                if (word != "") { //ensures blank DSStrings aren't added (which might otherwise happen if a tweet had two spaces in a row)
                    addPositiveWord(allWords, allWordsCount, word);
                }
            }

            word = message.substring(message.find(' ', numSpaces) + 1, (message.getLength() - message.find(' ', numSpaces))); //finds the last word

            if ((word != "\n") && (word != " ")) { //ensures DSStrings consisting of only newline characters or spaces aren't added
                addPositiveWord(allWords, allWordsCount, word);
            }
        } else { //for negative messages
            word = message.substring(0, message.find(' ')); //finds the first word

            addNegativeWord(allWords, allWordsCount, word); //adds word to list with a negative sentiment

            for (int i = 1; i < numSpaces; i++) { //loop that finds the middle words
                word = message.substring(message.find(' ', i) + 1, (message.find(' ', i + 1) - message.find(' ', i)) - 1);

                if (word != "") { //ensures blank DSStrings aren't added (which might otherwise happen if a tweet had two spaces in a row)
                    addNegativeWord(allWords, allWordsCount, word);
                }
            }

            word = message.substring(message.find(' ', numSpaces) + 1, (message.getLength() - message.find(' ', numSpaces))); //finds the last word

            if ((word != "\n") && (word != " ")) { //ensures DSStrings consisting of only newline characters or spaces aren't added
                addNegativeWord(allWords, allWordsCount, word);
            }
        }
        training_data >> sentiment;
    }

    training_data.close();

    for (int i = 0; i < allWordsCount.size(); i++) { //loop that removes all words with a low value (absolute value less than 10)
        if ((allWordsCount.at(i) < 10) && (allWordsCount.at(i) > -10)) { //removing these words speeds up the program (and a
            allWordsCount.erase(allWordsCount.begin() + i);              //score of less than 10 means the words are fairly neutral)
            allWords.erase(allWords.begin() + i);
            --i;
        }
    }

    ifstream test_data(argv[2], ios::in);
    ifstream test_data_answers(argv[3], ios::in);

    if (!test_data.is_open()) {
        cout << "Could not open testing file." << endl;
        return -1;
    }

    if (!test_data_answers.is_open()) {
        cout << "Could not open testing answers file." << endl;
        return -1;
    }

    vector<long int> testIDs; //vector that contains the IDs of every testing data tweet
    vector<int> predictions; //vector that contains the program's sentiment predictions
    vector<int> answers; //vector that contains the actual sentiment values of the testing data tweets

    test_data.getline(dummy, 50); //gets rid of first line
    test_data.getline(buffer, 400, ',');

    while (!test_data.eof()) {
        for (int i = 0; i < 3; i++) { //loop that skips useless data
            test_data.getline(dummy, 100, ',');
        }
        test_data.getline(buffer, 400); //reads in the tweet
        message = buffer;


        int numSpaces = message.count(' ');

        int positivity = 0; //a tweet's number of positive words
        int negativity = 0; //a tweet's number of negative words
        int wordScore; //each individual word's score


        word = message.substring(0, message.find(' '));
        wordScore = findWord(allWords, allWordsCount, word); //finds each word's sentiment (either positive, negative, or neutral)

        //neutral sentiments (if the word is not found in the program's database of words) are ignored
        if (wordScore > 0) { //activates if a word has a positive sentiment
            positivity++;
        } else if (wordScore < 0) { //activates if a word has a negative sentiment
            negativity++;
        }

        for (int i = 1; i < numSpaces; i++) {
            word = message.substring(message.find(' ', i) + 1, (message.find(' ', i + 1) - message.find(' ', i)) - 1);
            wordScore = findWord(allWords, allWordsCount, word); //finds each word's sentiment (either positive, negative, or neutral)

            if (wordScore > 0) { //activates if a word has a positive sentiment
                positivity++;
            } else if (wordScore < 0) { //activates if a word has a negative sentiment
                negativity++;
            }

        }

        word = message.substring(message.find(' ', numSpaces) + 1, (message.getLength() - message.find(' ', numSpaces)));
        wordScore = findWord(allWords, allWordsCount, word); //finds each word's sentiment (either positive, negative, or neutral)

        if (wordScore > 0) { //activates if a word has a positive sentiment
            positivity++;
        } else if (wordScore < 0) { //activates if a word has a negative sentiment
            negativity++;
        }

        if (positivity > negativity) { //if positivity is greater than negativity, then the program classifies it as a positive tweet
            predictions.push_back(4);
        } else { //if negativity is greater than positivity (or the two are equal), then the program classifies it as a negative tweet
            predictions.push_back(0);
        }

        test_data.getline(buffer, 400, ',');
    }

    test_data.close();

    test_data_answers.getline(dummy, 50); //gets rid of first line

    int sentimentAnswer;
    long int idNum;
    test_data_answers >> sentimentAnswer; //reads in the actual sentiment for each test tweet
    test_data_answers.ignore();
    test_data_answers >> idNum; //reads in the ID for each test tweet

    while (!test_data_answers.eof()) {
        answers.push_back(sentimentAnswer); //adds the actual sentiment to the answers vector
        testIDs.push_back(idNum); //adds the ID number of each test tweet to the testIDs vector

        test_data_answers >> sentimentAnswer;
        test_data_answers.ignore();
        test_data_answers >> idNum;
    }

    test_data_answers.close();

    double correct = 0.0;
    int total = 0;
    for (int i = 0; i < answers.size(); i++) { //loop to determine accuracy
        if (answers.at(i) == predictions.at(i)) { //activates if the prediction was correct
            correct += 1.0;
            total++;
        } else { //activates if the prediction was incorrect
            total++;
        }
    }

    ofstream outputFile;
    outputFile.open(argv[4], ios::out);
    outputFile << fixed << setprecision(3) << "Accuracy: " << (correct / total) << "\n"; //adds the accuracy to the output file
    for (int i = 0; i < testIDs.size(); i++) {
        if (predictions.at(i) != answers.at(i)) {
            outputFile << testIDs.at(i) << "\n"; //adds the IDs of the tweets whose sentiment was incorrectly guessed to the output file
        }
    }

    outputFile.close();
    return 0;
}

//the following function adds positive words to the allWords vector and updates each word's associated sentiment
void SentimentAnalyzer::addPositiveWord(vector<DSString>& allWords, vector<int>& allWordsCount, const DSString& word) {
    vector<DSString>::iterator it;
    it = find(allWords.begin(), allWords.end(), word); //searches through allWords to find the location of word

    if (it != allWords.end()) { //activates if word was found in allWords
        int index = distance(allWords.begin(), it);
        allWordsCount.at(index) += 1; //the word appearing in a positive tweet increases its value by 1
    } else { //activates if word was not found in allWords
        allWords.push_back(word); //adds word to allWords
        allWordsCount.push_back(1); //sets word's value to 1 (as it has positive sentiment)
    }
}

//the following function adds negative words to the allWords vector and updates each word's associated sentiment
void SentimentAnalyzer::addNegativeWord(vector<DSString>& allWords, vector<int>& allWordsCount, const DSString& word) {
    vector<DSString>::iterator it;
    it = find(allWords.begin(), allWords.end(), word); //searches through allWords to find the location of word

    if (it != allWords.end()) { //activates if word was found in allWords
        int index = distance(allWords.begin(), it);
        allWordsCount.at(index) -= 1; //the word appearing in a negative tweet decreases its value by 1
    } else { //activates if word was not found in allWords
        allWords.push_back(word); //adds word to allWords
        allWordsCount.push_back(-1); //sets word's value to -1 (as it has negative sentiment)
    }
}

//the following function searches for a word in allWords and returns its associated sentiment (or 0 if the word is not found)
int SentimentAnalyzer::findWord(vector<DSString> allWords, vector<int> allWordsCount, const DSString& word) {
    vector<DSString>::iterator it;
    it = find(allWords.begin(), allWords.end(), word); //searches through allWords to find the location of word

    if (it != allWords.end()) { //activates if word was found in allWords
        int index = distance(allWords.begin(), it);
        return allWordsCount.at(index); //returns the sentiment associated with the word
    }
    return 0; //returns 0 if the word was not found in allWords (indicating its sentiment is neutral)
}