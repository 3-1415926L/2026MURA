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
    int k = 2;
    int minUniqueIter = 2;
    int morphismSize = p;
    // minUniqueIter is the minimum number of iterations of a morphism
    //   to uniquely determine what symbol you started with based on
    //   the positions of the zeros in the iterated grid.

    vector<int> S = pkCantor(p, k, numIters);
    cout << "Created (" << p << "," << k
         << ")-Cantor Sequence (len = "
         << S.size() << ")" << endl;

    NumberWallSquare W{S, p};
    cout << "Created Number Wall" << endl;

    Morphism M{W.wall, morphismSize, minUniqueIter};
    cout << "Found Morphism (canon size = "
         << M.canonicalRules.size() << ") (full size = "
         << M.countSymbols() << ")" << endl;

    ofstream ff{"temp.txt"};
    M.printCanonicalMorphism(ff);
    cout << "\n=========================\n";
    M.printMorphism(ff);
    cout << "\n=========================\n";
    M.printCoding(ff);
    cout << "Saved Morphism to temp.txt" << endl;

    string startSymbol = M.units[1].getSymbol(); // M.units[1] = "0" rule
    Automatic2D A{toUnorderedMap(M.getRules()), startSymbol, numIters};
    cout << "Created Automatic Sequence" << endl;

    W.savePNG("a.png", pixelSize);
    cout << "Saved Number Wall image as a.png" << endl;
    A.savePNG("b.png", pixelSize);
    cout << "Saved Automatic Sequence image as b.png" << endl;

    return 0;
}