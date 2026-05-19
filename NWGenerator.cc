#include "NumberWallsV3.hpp"

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
    string sequence_file, temp;
    int max_width, pixel_size, modulo;
    ff >> temp >> sequence_file;
    ff >> temp >> max_width;
    ff >> temp >> pixel_size;
    ff >> temp >> modulo;
    string full_sequence_file = "sequences/" + sequence_file + ".txt";

    ff = ifstream{full_sequence_file};
    int i = 0, c = 0, x = 0;
    while ((ff >> x) && c < max_width) {
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

    string out_file = "imagesNumberWalls/" + sequence_file
                      + "_(w=" + to_string(c) + ")_(m="
                      + to_string(modulo) + ").png";
    W.savePNG(out_file , pixel_size, modulo);
    //cout << "\n\n\n" << *max_element(W.wall.begin(), W.wall.end());

    return 0;
}