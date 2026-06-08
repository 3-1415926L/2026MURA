#include "tools.hpp"
#include "MiscSequenceGenerators.hpp"
#include "NumberWalls.hpp"
#include "MorphismFinder.hpp"
#include "AutomaticSequences.hpp"

#include <vector>
#include <string>
#include <fstream>

using namespace std;



int main() {

    int pixelSize = 1;
    int numIters = 6;
    int p = 5;
    int k = 1;
    int minUniqueIter = 2;
    int morphismSize = p;
    // minUniqueIter is the minimum number of iterations of a morphism
    //   to uniquely determine what symbol you started with based on
    //   the positions of the zeros in the iterated grid.

    StopWatch SW;

    vector<int> S = pkCantor(p, k, numIters);
    cout << "Created (" << p << "," << k
         << ")-Cantor Sequence (len = "
         << S.size() << ")\n" << SW << endl << endl;

    NumberWallSquare W{S, p};
    cout << "Created Number Wall\n" << SW << endl << endl;

    Morphism M{W.wall, morphismSize, minUniqueIter};
    cout << "Found Morphism (canon size = "
         << M.canonicalRules.size() << ") (full size = "
         << M.countSymbols() << ")\n" << SW << endl << endl;

    ofstream ff{"temp---.txt"};
    M.printCanonicalMorphism(ff);
    ff << "\n=========================\n";
    M.printMorphism(ff);
    ff << "\n=========================\n";
    M.printCoding(ff);
    cout << "Saved Morphism to temp.txt\n" << SW << endl << endl;
    
    /*
    string startSymbol = M.units[1].getSymbol(); // M.units[1] = "0" rule
    Automatic2D A{toUnorderedMap(M.getRules()), startSymbol, numIters};
    cout << "Created Automatic Sequence\n" << SW << endl << endl;

    bool sameZeros1 = compareZeros(W.wall, A.grid);
    if (sameZeros1) cout << "Same zeros (1) = true\n" << SW << endl << endl;
    else cout << "Same zeros (1) = false\n" << SW << endl << endl;
    //*/

    bool sameZeros = M.compareZeros(W.wall);
    if (sameZeros) cout << "Same zeros = true\n" << SW << endl << endl;
    else cout << "Same zeros = false\n" << SW << endl << endl;

    /*
    W.savePNG("a.png", pixelSize);
    cout << "Saved Number Wall image as a.png\n" << SW << endl << endl;
    A.savePNG("b.png", pixelSize);
    cout << "Saved Automatic Sequence image as b.png\n" << SW << endl << endl;
    //*/

    return 0;
}