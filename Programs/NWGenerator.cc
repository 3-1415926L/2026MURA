#include "NumberWalls.hpp"

#include <iostream>
#include <vector>
#include <utility>
#include <fstream>
#include <string>

using namespace std;

int main() {
    vector<int> S;
    ifstream ff{"inputNumberWalls.txt"};
    // temp is a pointless string so that I can have notes in my input file
    string sequenceFile, temp;
    int maxWidth, pixelSize, modulo;
    ff >> temp >> sequenceFile;
    ff >> temp >> maxWidth;
    ff >> temp >> pixelSize;
    ff >> temp >> modulo;
    string fullSequenceFile = "sequences/" + sequenceFile + ".txt";

    ff = ifstream{fullSequenceFile};
    int i = 0, c = 0, x = 0;
    while ((ff >> x) && c < maxWidth) {
        if (i) {
            S.push_back(x);
            ++c;
        }
        i = 1 - i;
    }
    
    int len = S.size();

    NumberWall W{S, len, modulo};

    // uncomment line below to see text display of wall
    //W.printWall();

    string outFile = "imagesNumberWalls/" + sequenceFile
                      + "_(w=" + to_string(c) + ")_(m="
                      + to_string(modulo) + ").png";
    W.savePNG(outFile , pixelSize);
    //cout << "\n\n\n" << *max_element(W.wall.begin(), W.wall.end());
    cout << "valid=" << W.validWall() << endl;

    return 0;
}