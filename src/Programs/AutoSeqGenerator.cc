#include "AutomaticSequences.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>

using namespace std;



int main() {

    ifstream ff("inputAutomaticSequences.txt");
    int numIters, blockH, blockW, numSymbols;
    string startSymbol;

    // temp is a pointless string so that
    // I can have notes in my input file
    string temp;

    // read General information
    ff >> temp >> temp >> numIters;
    ff >> temp >> startSymbol;
    ff >> temp >> numSymbols;

    string out_file =
        "imagesAutomaticSequences/i="
        + to_string(numIters)
        + ",s="
        + startSymbol
        + ",r=(";

    // read Rules information
    ff >> temp >> temp >> blockH >> blockW;
    unordered_map<string, vector<vector<string>>> rules;

    for (int s = 0; s < numSymbols; ++s) {
        string symbol;
        ff >> symbol;
        if (s) out_file += ",";
        out_file += symbol + "-{";
        vector<vector<string>> block(
            blockH,
            vector<string>(blockW)
        );

        for (int r = 0; r < blockH; ++r) {
            if (r) out_file += ",";
            out_file += "{";
            for (int c = 0; c < blockW; ++c) {
                string element;
                ff >> element;
                block[r][c] = element;
                if (c) out_file += ",";
                out_file += element;
            }
            out_file += "}";
        }
        rules[symbol] = block;
        out_file += "}";
    }
    out_file += "),c=(";

    // read Coding information
    ff >> temp >> temp >> blockH >> blockW;
    unordered_map<string, vector<vector<int>>> coding;

    for (int s = 0; s < numSymbols; ++s) {
        string symbol;
        ff >> symbol;
        if (s) out_file += ",";
        out_file += symbol + "-{";
        vector<vector<int>> block(
            blockH,
            vector<int>(blockW)
        );

        for (int r = 0; r < blockH; ++r) {
            if (r) out_file += ",";
            out_file += "{";
            for (int c = 0; c < blockW; ++c) {
                int element;
                ff >> element;
                block[r][c] = element;
                if (c) out_file += ",";
                out_file += to_string(element);
            }
            out_file += "}";
        }
        coding[symbol] = block;
        out_file += "}";
    }
    out_file += ").png";

    // create the sequence and save it to a png
    Automatic2D A(rules, coding, startSymbol, numIters);

    // use if outputted file name is too long
    out_file = "imagesAutomaticSequences/a.png";
    
    A.savePNG(out_file, 1);

    return 0;
}