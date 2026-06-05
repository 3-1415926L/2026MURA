#include "NumberWalls.hpp"
#include "MorphismFinder.hpp"
#include "MiscSequenceGenerators.hpp"

#include <vector>
#include <string>

using namespace std;



int main() {

    int pixel_size = 1;
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

    NumberWallSquare W(S, p);
    cout << "Created Number Wall" << endl;

    Morphism M{W.wall, morphismSize, minUniqueIter};
    cout << "Found Morphism (canon size = "
         << M.canonicalRules.size() << ") (full size = "
         << M.countSymbols() << ")" << endl;

    ofstream ff{"temp.txt"};
    M.printCanonicalMorphism(ff);
    M.printMorphism(ff);
    M.printCoding(ff);

    return 0;
}