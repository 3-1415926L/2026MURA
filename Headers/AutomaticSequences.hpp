#pragma once

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>

using namespace std;



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

    // checks whether every symbol in the rules' output appears as an input
    bool verifyRules(unordered_map<int, vector<vector<int>>>& rules) {
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

    // assumes rules have been verified (see above)
    void iterate(unordered_map<int, vector<vector<int>>>& rules) {
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