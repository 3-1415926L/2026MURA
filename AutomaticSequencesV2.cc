#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>

using namespace std;



struct Automatic2D {
    vector<vector<int>> grid;
    int width;
    int height;

    Automatic2D(int startSymbol = 0): width{1}, height{1} {
        grid = {{startSymbol}};
    }

    int get(int r, int c) {
        return grid[r][c];
    }

    bool valid(int r, int c) {
        return r >= 0 && r < height && c >= 0 && c < width;
    }

    void iterate(unordered_map<int, vector<vector<int>>>& rules) {
        int blockH = rules.begin()->second.size();
        int blockW = rules.begin()->second[0].size();
        vector<vector<int>> next(
            height * blockH,
            vector<int>(width * blockW)
        );

        for (int r = 0; r < height; ++r) {
            for (int c = 0; c < width; ++c) {
                int symbol = grid[r][c];
                auto it = rules.find(symbol);

                if (it == rules.end()) {
                    cerr << "Missing rule for symbol "
                         << symbol << endl;
                    exit(1);
                }
                auto& block = it->second;

                for (int br = 0; br < blockH; ++br) {
                    for (int bc = 0; bc < blockW; ++bc) {
                        next[r * blockH + br]
                            [c * blockW + bc]
                            = block[br][bc];
                    }
                }
            }
        }
        grid = move(next);
        height = grid.size();
        width = grid[0].size();
    }

    void savePNG(string filename, int pixelSize, int maxNum) {

        int imgW = width * pixelSize;
        int imgH = height * pixelSize;
        vector<unsigned char> img(imgW * imgH * 3);

        for (int row = 0; row < height; ++row) {
            for (int col = 0; col < width; ++col) {
                int r, g, b;

                if (!valid(row, col)) {
                    r = g = b = 255;
                }
                else {

                    int x = get(row, col);

                    if (x == 0) {
                        r = 255;
                        g = 102;
                        b = 0;
                    }
                    else {
                        r = 0;
                        g = 0;
                        b = 50 + (205 * x) / maxNum;
                    }
                }

                for (int py = 0; py < pixelSize; ++py) {
                    for (int px = 0; px < pixelSize; ++px) {

                        int X = col * pixelSize + px;
                        int Y = row * pixelSize + py;
                        int idx = 3 * (Y * imgW + X);

                        img[idx + 0] = r;
                        img[idx + 1] = g;
                        img[idx + 2] = b;
                    }
                }
            }
        }

        stbi_write_png(
            filename.c_str(),
            imgW,
            imgH,
            3,
            img.data(),
            imgW * 3
        );
    }
};



unordered_map<string, int> symbolToId;
vector<string> idToSymbol;

int getSymbolId(const string& s) {
    auto it = symbolToId.find(s);
    if (it != symbolToId.end()) {
        return it->second;
    }
    
    int id = idToSymbol.size();
    symbolToId[s] = id;
    idToSymbol.push_back(s);
    return id;
}



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

    // create the sequence and save it to a png
    Automatic2D A(startSymbol);
    for (int i = 0; i < iterations; ++i) {
        A.iterate(rules);
    }
    A.iterate(coding);

    // use if outputted file name is too long
    //out_file = "imagesAutomaticSequences/a.png";
    
    A.savePNG(out_file, 2, maxNum);

    return 0;
}