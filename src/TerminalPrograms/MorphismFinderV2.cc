#include "NumberWalls.hpp"
#include "Morphisms.hpp"
#include "tools.hpp"

#include <iostream>
#include <vector>
#include <utility>
#include <fstream>
#include <string>
#include <algorithm>
#include <unordered_map>

using namespace std;



// REQUIRES SEQUENCE TO START AND END WITH A NON-ZERO VALUE!

StopWatch SW;

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



int main() {

    vector<int> S;
    ifstream ff{"inputNumberWalls.txt"};
    string testInput;
    if (!(ff >> testInput)) {
        cerr << "Could not find inputNumberWalls.txt" << endl;
        exit(1);
    }

    int modulo, seqOption;
    getInput(ff, "sequence_option", seqOption);
    getInput(ff, "modulo", modulo);

    int morphismSize, minUniqueIter;
    getInput(ff, "morphism_size", morphismSize);
    getInput(ff, "min_iters_for_unique", minUniqueIter);

    // get sequence

    if (seqOption == 1) {
        // Option 1 - A sequence in the sequences folder
        skipTo(ff, "Option_1");

        string sequenceFile;
        int maxWidth;
        getInput(ff, "sequence_name", sequenceFile);
        getInput(ff, "max_width", maxWidth);
        sequenceFile = "sequences/" + sequenceFile + ".txt";
        
        ifstream ffSeq{sequenceFile};
        int element;
        int count = 0;
        while ((ffSeq >> testInput) && count < maxWidth) {
            ffSeq >> element;
            S.push_back(element);
            ++count;
        }
        
        if (S.size() == 0) {
            cerr << "Could not read from " << sequenceFile << endl;
        }

    } else if (seqOption == 2) {
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

    } else if (seqOption == 3) {
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

        for (auto& [symbol, rule] : morphism) {
            for (int element : rule) {
                if (!morphism.contains(element)) {
                    cerr << "No rule given for symbol " << element << endl;
                    exit(1);
                }
            }
        }

        S = vector{startSymbol};
        for (int i = 0; i < numIter; ++i) {
            applyMorphism(S, morphism);
        }

    } else if (seqOption == 4) {
        // Option 4 - From a custom sequence
        skipTo(ff, "Option_4");

        skipTo(ff, "sequence_(below):");
        int element;
        while (ff >> element) {
            S.push_back(element);
        }

    } else {
        cerr << "Sequence generation option " << seqOption
             << " does not exist" << endl;
        exit(1);
    }

    cout << "Sequence generated" << endl;
    SW.printAndReset();
    cout << endl << endl;
    
    // make the Number Wall and find its morphism

    try {
        SquareWallMorphism SWM{S, modulo, morphismSize, minUniqueIter};
        
        cout << "Found Morphism (canon size = "
            << SWM.canonicalRules.size() << ") (full size = "
            << SWM.countSymbols() << ")\n" << SW << endl << endl;

        ofstream ffM{"temp.txt"};
        ffM << "Canonical morphism\n\n";
        SWM.printCanonicalMorphism(ffM);
        ffM << "\n=========================\n\nFull morphism:\n\n";
        SWM.printMorphism(ffM);
        ffM << "\n=========================\n\nCoding:\n\n";
        SWM.printCoding(ffM);
        cout << "Saved Morphism to temp.txt\n" << SW << endl << endl;

        /*
        bool sameZeros = M.compareZeros(W.wall);
        if (sameZeros) cout << "The morphism PASSED the verification test" << endl;
        else cout << "The morphism FAILED the verification test" << endl
                  << "Try changing parameters or including more of the sequence" << endl;
        //*/
    } catch (const string& s) {
        cout << "Morphism not found" << endl
             << "Try changing parameters or including more of the sequence" << endl;
    }//*/

    return 0;
}