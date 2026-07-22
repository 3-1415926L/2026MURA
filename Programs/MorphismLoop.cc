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

// 0 = success
// 1 = found morphism, but didn't pass check
// 2 = didn't find morphism
void findNWMorphism(NumberWall<FlatSquareLayout>& W, int modulo, int morphismSize, int numIter,
        int minUniqueIter, string bitPattern, bool printDetails = 0, bool saveImage = 0) {

    ostringstream tempOut;

    tempOut << "Number Wall created" << endl;
    SW.printAndReset(tempOut);
    tempOut << endl << endl;

    Morphism M{W.wall, morphismSize, minUniqueIter};
    
    tempOut << "Found Morphism (canon size = "
        << M.canonicalRules.size() << ") (full size = "
        << M.countSymbols() << ")\n" << SW << endl << endl;
    
    //*
    bool sameZeros = M.compareZeros(W.wall);
    if (sameZeros) {
        tempOut << "The morphism PASSED the verification test" << endl;
        SW.printAndReset(tempOut);
    }
    else throw string("failed verification");

    string morphismFile = "MorphismLoopOutput/" + bitPattern + "(m=" + to_string(modulo) + ").txt";
    ofstream ff{morphismFile};
    ff << "Canonical morphism\n\n";
    M.printCanonicalMorphism(ff);
    ff << "\n=========================\n\nFull morphism:\n\n";
    M.printMorphism(ff);
    ff << "\n=========================\n\nCoding:\n\n";
    M.printCoding(ff);

    tempOut << "Saved Morphism to temp.txt\n" << SW << endl << endl;

    if (saveImage) {
        try {
            string imgFile = "MorphismLoopOutput/" + bitPattern + "(m="
                + to_string(modulo) + ")(i=" + to_string(numIter) + ").png";
            W.savePNG(imgFile, 1);
            tempOut << "Wall image saved" << endl;
            SW.printAndReset(tempOut);
            tempOut << endl << endl;
        } catch (...) {
            tempOut << "failed to save wall image" << endl;
            SW.printAndReset(tempOut);
            tempOut << endl << endl;
        }
    }

    // if made it this far without exceptions/errors, then print info
    if (printDetails) {
        cout << tempOut.str();
    }
}



// Can change these:

bool saveImage = 1;
vector<int> modulos = {2, 3, 5, 7};
int startLen = 2;
int maxWidth = 40000;
bool printDetails = 0;



int main() {

    unordered_map<int, vector<int>> morphism;
    morphism[0] = vector<int>(startLen - 1, 0);
    
    for (int len = startLen;; ++len) { // shorter morphisms first
        morphism[0].push_back(0);
        int morphismSize = len;

        // loop through possible morphisms of length len
        for (int mask = 0; mask < (1 << len); ++mask) {

            vector<int> v;
            string bitPattern;
            bool has0 = false;
            bool has1 = false;

            for (int i = 0; i < len; ++i) {
                int bit = (mask >> (len - 1 - i)) & 1;
                v.push_back(bit);
                bitPattern += char('0' + bit);

                if (bit == 0) has0 = true;
                else has1 = true;
            }

            if (!has0 || !has1) continue;

            /////////////////// maybe remove this later
            if (!v[0] || !v.back()) continue;

            // use v as the morphism for 1
            morphism[1] = v;

            cout << endl << "================ 1->" << bitPattern << " (Msize="
                 << morphismSize << ")" << endl;
            
            for (int modulo : modulos) {

                cout << "m=" << modulo << " ";

                // for each rule for 1 and each modulo, attempt to find a morphism

                vector<int> S = vector{1};
                applyMorphism(S, morphism);
                
                bool found = false;
                bool skipToNext = false;

                for (int numIter = 2; S.size() * morphismSize < maxWidth; ++numIter) {
                    applyMorphism(S, morphism);

                    if (found || skipToNext) break;

                    /////cout << "--- numIter=" << numIter << endl << endl;
                    
                    // make the wall outside of the minUniqueIter loop
                    NumberWall<FlatSquareLayout> W{S, modulo};

                    for (int minUniqueIter = 1; minUniqueIter < numIter; ++minUniqueIter) {
                        
                        /////cout << "minUniqueIter=" << minUniqueIter << endl;

                        try {
                            findNWMorphism(W, modulo, morphismSize, numIter, minUniqueIter, bitPattern, printDetails, saveImage);
                            found = true;
                            cout << "FOUND ";
                            break;
                        }
                        catch (const string& s) {
                            if (s == "failed verification") {
                                continue; // next minUniqueIter
                            } else if (s == "Rule Not Found") {
                                break; // next numIter
                            } else if (s == "New Unit found past threshold") {
                                break; // next numIter
                            } else {
                                throw s;
                            }
                        }
                    }
                }

                /*
                if (!found) {
                    cout << "Morphism not found with alloted resources" << endl << endl;
                }
                */
                // onto next modulo
            }
            // onto next bit pattern
        }
    }

    return 0;
}