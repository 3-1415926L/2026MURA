#include "AutomaticSequences.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>

using namespace std;



int main() {

    ifstream ff("inputAutomaticSequences.txt");
    int iterations, blockH, blockW, numSymbols;
    string startSymbolString;

    // temp is a pointless string so that
    // I can have notes in my input file
    string temp;

    // read General information
    ff >> temp >> temp >> iterations;
    ff >> temp >> startSymbolString;
    ff >> temp >> numSymbols;
    int startSymbol = getSymbolId(startSymbolString);

    string out_file =
        "imagesAutomaticSequences/i="
        + to_string(iterations)
        + ",s="
        + startSymbolString
        + ",r=(";

    // read Rules information
    ff >> temp >> temp >> blockH >> blockW;
    unordered_map<int, vector<vector<int>>> rules;

    for (int s = 0; s < numSymbols; ++s) {
        string symbolString;
        ff >> symbolString;
        int symbol = getSymbolId(symbolString);
        if (s) out_file += ",";
        out_file += symbolString + "-{";
        vector<vector<int>> block(
            blockH,
            vector<int>(blockW)
        );

        for (int r = 0; r < blockH; ++r) {
            if (r) out_file += ",";
            out_file += "{";
            for (int c = 0; c < blockW; ++c) {
                string elementString;
                ff >> elementString;
                int element = getSymbolId(elementString);
                block[r][c] = element;
                if (c) out_file += ",";
                out_file += elementString;
            }
            out_file += "}";
        }
        rules[symbol] = block;
        out_file += "}";
    }
    out_file += "),c=(";

    // read Coding information
    ff >> temp >> temp >> blockH >> blockW;
    unordered_map<int, vector<vector<int>>> coding;
    int maxNum = 0;

    for (int s = 0; s < numSymbols; ++s) {
        string symbolString;
        ff >> symbolString;
        int symbol = getSymbolId(symbolString);
        if (s) out_file += ",";
        out_file += symbolString + "-{";
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
                if (element > maxNum) maxNum = element;
                if (c) out_file += ",";
                out_file += to_string(element);
            }
            out_file += "}";
        }
        coding[symbol] = block;
        out_file += "}";
    }
    out_file += ").png";


/*// Uncomment to see rules
    cout << "\nrules:\n\n";
    for (int s = 0; s < numSymbols; ++s) {
        cout << idToSymbol[s] << ":" << endl;
        for (int r = 0; r < 5; ++r) {
            for (int c = 0; c < 5; ++c) {
                cout << idToSymbol[rules[s][r][c]] << " ";
            }
            cout << endl;
        }
        cout << endl << endl;
    }
    for (int s = 0; s < numSymbols; ++s) {
        cout << idToSymbol[s] << ":" << endl;
        for (int r = 0; r < 1; ++r) {
            for (int c = 0; c < 1; ++c) {
                cout << idToSymbol[coding[s][r][c]] << " ";
            }
            cout << endl;
        }
        cout << endl << endl;
    }
//*/


    // create the sequence and save it to a png
    Automatic2D A(startSymbol);
    for (int i = 0; i < iterations; ++i) {
        A.iterate(rules);
    }
    A.iterate(coding);

    // use if outputted file name is too long
    out_file = "imagesAutomaticSequences/a.png";
    
    A.savePNG(out_file, 1, maxNum);

    return 0;
}