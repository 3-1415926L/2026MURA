#include "NumberWalls.hpp"

#include <iostream>
#include <vector>
#include <utility>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;



void skipTo(ifstream& ff, string inputName) {
    string x;
    while (ff >> x) {
        if (x == inputName) {
            return;
        }
    }
    cerr << "Input name " << inputName << " not found" << endl;
    exit(1);
}

template<typename T>
void getInput(ifstream& ff, string inputName, T& variable){
    if (!inputName.ends_with(':')) {
        inputName += ":";
    }
    skipTo(ff, inputName);
    if (!(ff >> variable)) {
        cerr << "Input for " << inputName << " invalid" << endl;
        exit(1);
    }
}

void applyMorphism(vector<int>& S, unordered_map<int, vector<int>>& morphism) {
    vector<int> result;
    for (auto symbol : S) {
        result.insert(result.end(), morphism[symbol].begin(), morphism[symbol].end());
    }
    S = move(result);
}

template<typename WallType>
void doNumberWallStuff(WallType& W, int pixelSize) {
    if (!(W.validWall())) {
        cerr << "Number wall was constructed invalidly" << endl;
        exit(1);
    }
    string outFile = "a.png";
    //W.printWall();
    W.savePNG(outFile , pixelSize);
    //cout << "\n\n\n" << *max_element(W.wall.begin(), W.wall.end());
}

int main() {
    vector<int> S;
    ifstream ff{"inputNumberWalls.txt"};
    string testInput;
    if (!(ff >> testInput)) {
        cerr << "Could not find inputNumberWalls.txt" << endl;
        exit(1);
    }
    int pixelSize, modulo, genOption;
    getInput(ff, "pixel_size", pixelSize);
    getInput(ff, "modulo", modulo);
    getInput(ff, "sequence_generation_option", genOption);

    if (genOption == 1) {
        // Option 1 - A sequence in the sequences folder
        skipTo(ff, "Option_1");

        string sequenceFile;
        int maxWidth;
        getInput(ff, "sequence_name", sequenceFile);
        getInput(ff, "max_width", maxWidth);
        sequenceFile = "sequences/" + sequenceFile + ".txt";
        
        ifstream ffSeq{sequenceFile};
        int element;
        int count;
        while ((ffSeq >> testInput) && count < maxWidth) {
            ffSeq >> element;
            S.push_back(element);
            ++count;
        }
        
        if (S.size() == 0) {
            cerr << "Could not read from " << sequenceFile << endl;
        }

    } else if (genOption == 2) {
        // Option 2 - A sequence in a specified file
        skipTo(ff, "Option_2");

        string sequenceFile;
        int maxWidth;
        getInput(ff, "sequence_name", sequenceFile);
        getInput(ff, "max_width", maxWidth);
        
        ifstream ffSeq{sequenceFile};
        int element;
        int count;
        while ((ffSeq >> testInput) && count < maxWidth) {
            ffSeq >> element;
            S.push_back(element);
            ++count;
        }
        
        if (S.size() == 0) {
            cerr << "Could not read from " << sequenceFile << endl;
        }

    } else if (genOption == 3) {
        // Option 3 - From a given morphism
        skipTo(ff, "Option_3");

        int startSymbol, numIter;
        getInput(ff, "start_symbol", startSymbol);
        getInput(ff, "number_of_iterations", numIter);
        skipTo(ff, "morphism_(below):");

        unordered_map<int, vector<int>> morphism;
        int symbol, element;
        string line;
        while (ff >> symbol) {

            getline(ff, line); // this line is just the \n after symbol
            getline(ff, line);
            istringstream ll{line};
            while (ll >> element) {
                morphism[symbol].push_back(element);
            }
            if (morphism[symbol].size() == 0) {
                cerr << "No rule given for symbol " << symbol << endl;
                exit(1);
            }
        }

        S = vector{startSymbol};
        for (int i = 0; i < numIter; ++i) {
            applyMorphism(S, morphism);
        }

    } else if (genOption == 4) {
        // Option 4 - From a custom sequence
        skipTo(ff, "Option_4");

        skipTo(ff, "sequence_(below):");
        int element;
        while (ff >> element) {
            S.push_back(element);
        }

    } else {
        cerr << "Option " << genOption << " does not exist";
        exit(1);
    }
    
    if (modulo == 0) {
        NumberWallNoMod W{S};
        doNumberWallStuff(W, pixelSize);
    } else {
        NumberWall W{S, modulo};
        doNumberWallStuff(W, pixelSize);
    }

    return 0;
}