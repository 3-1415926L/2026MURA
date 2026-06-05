#pragma once

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;



struct Automatic2D {
    vector<vector<int>> grid;
    int width;
    int height;
    unordered_map<string, int> symbolToId;
    vector<string> idToSymbol;
    unordered_map<int, vector<vector<int>>> rules;
    unordered_map<int, vector<vector<int>>> coding;

    Automatic2D(unordered_map<string, vector<vector<string>>> rulesString,
                unordered_map<string, vector<vector<int>>> codingString,
                string& startString, int numIters): width{1}, height{1} {
        // create symbolToId and idToSymbol
        // assumes each symbol is unique
        int c = 0;
        for (auto& [symbol, rule] : rulesString) {
            idToSymbol.push_back(symbol);
            symbolToId[symbol] = c;
            ++c;
        }

        // get starting symbol
        if (symbolToId.find(startString) == symbolToId.end()) {
            cout << "Error! Invalid start symbol" << endl;
        }
        int startInt = symbolToId[startString];

        // create rules map
        int blockH = rulesString[startString].size();
        int blockW = rulesString[startString][0].size();
        for (auto& [symbol, rule] : rulesString) {
            vector<vector<int>> block;
            for (int r = 0; r < blockH; ++r) {
                block.push_back({});
                for (int c = 0; c < blockW; ++c) {
                    block[r].push_back(symbolToId[rule[r][c]]);
                }
            }
            rules[symbolToId[symbol]] = block;
        }

        // create coding map
        int codingH = codingString[startString].size();
        int codingW = codingString[startString][0].size();
        for (auto& [symbol, code] : codingString) {
            vector<vector<int>> block;
            for (int r = 0; r < codingH; ++r) {
                block.push_back({});
                for (int c = 0; c < codingW; ++c) {
                    block[r].push_back(code[r][c]);
                }
            }
            coding[symbolToId[symbol]] = block;
        }

        // iterate rules numIters times
        grid = {{startInt}};
        for (int i = 0; i < numIters; ++i) {
            applyRules();
        }
        applyCoding();
    }
/*
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
*/
    int get(int r, int c) {
        return grid[r][c];
    }

    bool valid(int r, int c) {
        return r >= 0 && r < height && c >= 0 && c < width;
    }

    // checks whether every symbol in the rules' output appears as an input
    bool verifyRules() {
        for (auto [symbol, rule] : rules) {
            for (auto row : rule) {
                for (auto element : row) {
                    if (rules.find(element) == rules.end()) {
                        return false;
                        cout << "Missing rule for symbol "
                             << idToSymbol[symbol] << endl;
                    }
                }
            }
        }
        return true;
    }

    void applyRules() {apply(rules);}
    void applyCoding() {apply(coding);}

    // assumes rules have been verified (see above)
    void apply(unordered_map<int, vector<vector<int>>>& rules) {
        int blockH = rules.begin()->second.size();
        int blockW = rules.begin()->second[0].size();
        vector<vector<int>> next(
            height * blockH,
            vector<int>(width * blockW)
        );
        int rIndex, cIndex;

        for (int r = 0; r < height; ++r) {
            rIndex = r * blockH;
            for (int c = 0; c < width; ++c) {
                cIndex = c * blockW;
                int symbol = grid[r][c];
                auto& block = rules[symbol];

                for (int br = 0; br < blockH; ++br) {
                    for (int bc = 0; bc < blockW; ++bc) {
                        next[rIndex + br][cIndex + bc] = block[br][bc];
                    }
                }
            }
        }
        grid = move(next);
        height = grid.size();
        width = grid[0].size();
    }

    void savePNG(string filename, int pixelSize) {
        // get maxNum
        int maxNum = 0;
        for (auto& [symbol, code] : coding) {
            for (auto& row : code) {
                for (auto element : row) {
                    if (element > maxNum) maxNum = element;
                }
            }
        }

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