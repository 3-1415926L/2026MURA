#include "tools.hpp"
#include "MiscSequenceGenerators.hpp"
#include "NumberWalls.hpp"
#include "Morphisms.hpp"
#include "AutomaticSequences.hpp"

#include <vector>
#include <string>
#include <fstream>

using namespace std;



int main() {

    int pixelSize = 1;
    int numIters = 6;
    int p = 7;
    int k = 3;
    int minUniqueIter = 2;
    int morphismSize = p;
    vector<int> S;
    // minUniqueIter is the minimum number of iterations of a morphism
    //   to uniquely determine what symbol you started with based on
    //   the positions of the zeros in the iterated grid.

    //*
    cout << "Enter the predicted morphism size\n"
         << "(morphism is assumed to be square)\n"
         << "(for (p,k)-Cantor, p is recommended)\n";
    cin >> morphismSize;

    cout << "Enter the predicted number of iterations to uniquely "
         << "determine a symbol by its zero entries\n"
         << "(For example, with A -> 0B, B -> 0C, C-> CC, A and B map to the "
         << "same zero pattern, but their second iterations are different)\n"
         << "(for (p,k)-Cantor, 2 is recommended)\n";
    cin >> minUniqueIter;

    cout << "Enter the predicted number of iterations before "
         << "every unique pattern of zeros has been seen\n"
         << "(up to rotation/reflection)\n"
         << "(for (p,k)-Cantor, 5 is recommended)\n";
    cin >> numIters;
    numIters++; // need one more iteration to get the rules for these symbols

    string usingPKCantorString;
    cout << "are you finding the morphism of a (p,k)-Cantor sequence (y or n)\n";
    cin >> usingPKCantorString;
    bool usingPKCantor = (usingPKCantorString == "y");
    if (usingPKCantor) {
        cout << "Enter the values for p and k.\n"
             << "(p must be prime and one more than a multiple of 2k)\n"
             << "(See README for definition of (p,k)-Cantor sequence)\n";
        cin >> p >> k;
    } else {
        cout << "enter the sequence name you would like to find a morphism for\n";
        string sequenceFile;
        cin >> sequenceFile;
        string fullSequenceFile = "sequences/" + sequenceFile + ".txt";
        ifstream ff{fullSequenceFile};
        int i = 0, c = 0, x = 0;
        int maxWidth = pow(morphismSize, numIters);
        while ((ff >> x) && c < maxWidth) {
            if (i) {
                S.push_back(x);
                ++c;
            }
            i = 1 - i;
        }
    }
    //*/

    StopWatch SW;

    if (usingPKCantor) {
        S = pkCantor(p, k, numIters);
        cout << "Created (" << p << "," << k
             << ")-Cantor Sequence (len = "
             << S.size() << ")\n" << SW << endl << endl;
    }

    NumberWall<NestedSquareLayout> W{S, p};
    cout << "Created Number Wall\n" << SW << endl << endl;

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
    
    /*
    string startSymbol = M.units[1].getSymbol(); // M.units[1] = "0" rule
    Automatic2D A{toUnorderedMap(M.getRules()), startSymbol, numIters};
    cout << "Created Automatic Sequence\n" << SW << endl << endl;

    bool sameZeros1 = compareZeros(W.wall, A.grid);
    if (sameZeros1) cout << "Same zeros = true\n" << SW << endl << endl;
    else cout << "Same zeros = false\n" << SW << endl << endl;
    //*/

    //*
    bool sameZeros = M.compareZeros(W.wall);
    if (sameZeros) cout << "Same zeros = true\n" << SW << endl << endl;
    else cout << "Same zeros = false\n" << SW << endl << endl;
    //*/
} catch (...) {
    cout << "Morphism not found. Try changing parameters.\n\n";
}
    //*
    W.savePNG("a.png", pixelSize);
    cout << "Saved Number Wall image as a.png\n" << SW << endl << endl;
    //A.savePNG("b.png", pixelSize);
    //cout << "Saved Automatic Sequence image as b.png\n" << SW << endl << endl;
    //*/

    return 0;
}