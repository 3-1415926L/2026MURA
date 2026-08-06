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

template<typename WallType>
void findMorphism(WallType& W, int morphismSize, int minUniqueIter) {

    try {
        Morphism M{W.wall, morphismSize, minUniqueIter};
        
        cout << "Found Morphism (canon size = "
            << M.canonicalRules.size() << ") (full size = "
            << M.countSymbols() << ")\n" << SW << endl << endl;

        ofstream ff{"temp.txt"};
        ff << "Canonical morphism\n\n";
        M.printCanonicalMorphism(ff);
        ff << "\n=========================\n\nFull morphism:\n\n";
        M.printMorphism(ff);
        ff << "\n=========================\n\nCoding:\n\n";
        M.printCoding(ff);
        cout << "Saved Morphism to temp.txt\n" << SW << endl << endl;

        //*
        bool sameZeros = M.compareZeros(W.wall);
        if (sameZeros) cout << "The morphism PASSED the verification test" << endl;
        else cout << "The morphism FAILED the verification test" << endl
                  << "Try changing parameters or including more of the sequence" << endl;
        //*/
    } catch (...) {
        cout << "Morphism not found" << endl
             << "Try changing parameters or including more of the sequence" << endl;
    }
}

template<typename WallType>
void doNumberWallStuff(WallType& W,
        unordered_map<string, bool>& NWfeatures,
        unordered_map<string, int>& NWargs) {

    cout << "Number Wall created" << endl;
    SW.printAndReset();
    cout << endl << endl;

    if (NWfeatures["print_text"]) { // print_text
        W.printWall();
        cout << "Wall printed" << endl;
        SW.printAndReset();
        cout << endl << endl;
    }
    if (NWfeatures["save_image"]) { // save_image
        W.savePNG("a.png", NWargs["pixel_size"]);
        cout << "Wall image saved" << endl;
        SW.printAndReset();
        cout << endl << endl;
    }
    if (NWfeatures["find_morphism"]) { // find_morphism
        findMorphism(W, NWargs["morphism_size"], NWargs["min_iters_for_unique"]);
        SW.printAndReset();
        cout << endl << endl;
    }
    if (NWfeatures["max_element"]) { // find_morphism
        cout << "Maximum element in wall = " << W.getMaxNum() << endl;
        SW.printAndReset();
        cout << endl << endl;
    }
    if (NWfeatures["max_zero_size"]) { // find_morphism
        cout << "Maximum zero square size = " << W.maxZeroSize() << endl;
        SW.printAndReset();
        cout << endl << endl;
    }
    if (NWfeatures["check_FC1"]) { // find_morphism
        bool FC1 =  W.checkFC1();
        if (FC1) {
            cout << "The wall has PASSED the FC1 check" << endl;
        } else {
            cout << "The wall has FAILED the FC1 check" << endl;
        }
        SW.printAndReset();
        cout << endl << endl;
    }
}



int main() {

    vector<int> S;
    ifstream ff{"inputNumberWalls.txt"};
    string testInput;
    if (!(ff >> testInput)) {
        cerr << "Could not find inputNumberWalls.txt" << endl;
        exit(1);
    }

    int modulo, seqOption, NWtype;
    bool usingSquareWall;
    getInput(ff, "sequence_option", seqOption);
    getInput(ff, "modulo", modulo);
    getInput(ff, "number_wall_type", NWtype);
    getInput(ff, "square_wall", usingSquareWall);

    unordered_map<string, bool> NWfeatures;
    bool feature;
    getInput(ff, "print_text", feature);
    NWfeatures["print_text"] = feature;

    getInput(ff, "save_image", feature);
    NWfeatures["save_image"] = feature;

    getInput(ff, "find_morphism", feature);
    NWfeatures["find_morphism"] = feature;

    getInput(ff, "max_element", feature);
    NWfeatures["max_element"] = feature;

    getInput(ff, "max_zero_size", feature);
    NWfeatures["max_zero_size"] = feature;

    getInput(ff, "check_FC1", feature);
    NWfeatures["check_FC1"] = feature;
    
    if (NWfeatures["find_morphism"] && !usingSquareWall) {
        cerr << "Must be using a square numberwall to search for a morphism" << endl;
        exit(1);
    }
    
    unordered_map<string, int> NWargs;
    int arg;
    if (NWfeatures["find_morphism"]) {
        getInput(ff, "morphism_size", arg);
        NWargs["morphism_size"] = arg;
        
        getInput(ff, "min_iters_for_unique", arg);
        NWargs["min_iters_for_unique"] = arg;
    }
    if (NWfeatures["save_image"]) {
        getInput(ff, "pixel_size", arg);
        NWargs["pixel_size"] = arg;
    }

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
            exit(1);
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
        int count = 0;
        while ((ffSeq >> testInput) && count < maxWidth) {
            ffSeq >> element;
            S.push_back(element);
            ++count;
        }
        
        if (S.size() == 0) {
            cerr << "Could not read from " << sequenceFile << endl;
            exit(1);
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

    // zeros at the start/end of the sequence would be irrelevant in this case
    if (usingSquareWall) {
        int firstNonZero = 0;
        while (firstNonZero < S.size() && S[firstNonZero] == 0) {
            firstNonZero++;
        }
        int lastNonZero = S.size();
        while (lastNonZero > firstNonZero && S[lastNonZero - 1] == 0)
            lastNonZero--;

        S = vector<int>(S.begin() + firstNonZero, S.begin() + lastNonZero);
    }

    cout << "Sequence generated" << endl;
    SW.printAndReset();
    cout << endl << endl;
    
    // make the Number Wall
    
    if (modulo == 0) {
        if (NWtype != 1 || usingSquareWall) {
            cerr << "This Number Wall type currently does not support modulus 0" << endl;
            exit(1);
        }
        NumberWallNoMod W{S};
        doNumberWallStuff(W, NWfeatures, NWargs);
    }
    else if (usingSquareWall) {
        if (NWtype == 1) {
            NumberWall<FlatSquareLayout> W{S, modulo};
            doNumberWallStuff(W, NWfeatures, NWargs);
        } else if (NWtype == 2) {
            NumberWallDet<FlatSquareLayout> W{S, modulo};
            doNumberWallStuff(W, NWfeatures, NWargs);
        } else if (NWtype == 3) {
            NumberWallPermRyser<FlatSquareLayout> W{S, modulo};
            doNumberWallStuff(W, NWfeatures, NWargs);
        } else if (NWtype == 4) {
            NumberWallDet3D<FlatSquareLayout> W{S, modulo};
            doNumberWallStuff(W, NWfeatures, NWargs);
        } else {
            cerr << "Number Wall type " << NWtype
                << " does not exist" << endl;
            exit(1);
        }
    } // if usingSquareWall

    else { // if not usingSquareWall
        if (NWtype == 1) {
            NumberWall W{S, modulo};
            doNumberWallStuff(W, NWfeatures, NWargs);
        } else if (NWtype == 2) {
            NumberWallDet W{S, modulo};
            doNumberWallStuff(W, NWfeatures, NWargs);
        } else if (NWtype == 3) {
            NumberWallPermRyser W{S, modulo};
            doNumberWallStuff(W, NWfeatures, NWargs);
        } else if (NWtype == 4) {
            NumberWallDet3D W{S, modulo};
            doNumberWallStuff(W, NWfeatures, NWargs);
        } else {
            cerr << "Number Wall type " << NWtype
                << " does not exist" << endl;
            exit(1);
        }
    } // if not usingSquareWall
    

    return 0;
}