#pragma once

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "tools.hpp"

#include <iostream>
#include <vector>
#include <utility>
#include <cmath>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;



struct NumberWall {
    vector<int> wall; // does not include row -1 or -2
    vector<int> offset;
    int width;
    int height; // does not include row -1 or -2
    int modulo;
    vector<int> inverse;

    // ctor
    NumberWall(vector<int> S, int w, int modulo): width{w}, height{(w + 1) / 2}, modulo{modulo} {
        int total = 0;
        for (int n = w; n >= 1; n -= 2) {
            offset.push_back(total);
            total += n;
        }
        wall.resize(total);
        for (int i = 0; i < modulo; ++i) {
            inverse.push_back(modPow(i, modulo - 2, modulo));
        }
        makeWall(S);
    }

    // both of these require row <= col < width - row
    virtual int get(int row, int col) {
        if (row == -1) return 1;
        if (row < -1) return 0;
        return wall[offset[row] + col - row];
    }

    // Sets entries at row, col to value
    virtual void set(int row, int col, int value) {
        wall[offset[row] + col - row] = value;
    }

    // Sets entries at row, [colStart, colEnd) to value
    virtual void setRange(int row, int colStart, int colEnd, int value) {
        while (colStart < colEnd) {
            wall[offset[row] + colStart - row] = value;
            ++colStart;
        }
    }

    virtual void makeWall(vector<int> S) {
        // zero/one rows implicitly there
        // create sequence row
        for(int i = 0; i < width; ++i) {
            wall[i] = (modulo + (S[i] % modulo)) % modulo;
        }
        // create other rows
        int row = 1, col = 1;
        while (row < height) {
            col = row;
            while (col < width - row) {

                if (get(row - 2, col) != 0) {
                    int x = (((get(row - 1, col) * get(row - 1, col))
                        - (get(row - 1, col - 1) * get(row - 1, col + 1)))
                        * inverse[get(row - 2, col)]);
                    set(row, col, (modulo + (x % modulo)) % modulo);
                    ++col;
                    continue;
                }

                // else D_k or H_k (i.e., zero square above)
                // top, left, & right = row/cols just outside zero square
                int top = row - 2, left = col, right = col;
                while (get(top - 1, col) == 0) {
                    top--;
                }
                if (row == col) { // if first col in row, else can assume already at left of zero square
                    while (valid(top, left - 1) && get(top, left - 1) == 0) {
                        left--;
                    }
                }
                while (valid(top, right + 1) && get(top, right + 1) == 0) {
                    right++;
                }
                top--;
                left--;
                right++;

                // if 0 square extends to edge, W[row,col] must be 0
                if (!(valid(top,left) && valid(top,right))) {
                    setRange(row, col, min(right, width - col), 0);
                    col = right;
                    continue;
                }

                // same here
                if (right - left > row - top) {
                    setRange(row, col, min(right, width - col), 0);
                    col = right;
                    continue;
                }

                // else, D_k or H_k
                int l = right - left - 1;
                int k = right - col;
                int Ak = get(top, left + k);
                int Bk = get(top + k, left);
                int Ck = get(top + l + 1 - k, right);

                int rA = get(top, left + 1) * inverse[get(top, left)] % modulo;
                int rB = get(top + 1, left) * inverse[get(top, left)] % modulo;
                int rC = get(top, right) * inverse[get(top + 1, right)] % modulo;
                // uses relation between rA, rB, rC, and rD
                int rD = (1 - 2 * (l % 2)) * rB * rC * inverse[rA];
                rD = (modulo + (rD % modulo)) % modulo;
                int rDinv = inverse[rD];

                // D_k
                if (get(row - 1, col) == 0) {
                    int Dk;
                    if (row == col) Dk = (1 - 2 * (k % 2 && l % 2)) * Bk * Ck * inverse[Ak];
                    else Dk = get(row, col - 1) * rDinv;
                    Dk = (modulo + (Dk % modulo)) % modulo;
                    set(row, col, Dk);
                    ++col;

                    // uses fact that Dk is a geometric sequence to create whole row at once
                    while (col <= right && col < width - row) {
                        Dk = Dk * rDinv % modulo;
                        set(row, col, Dk);
                        ++col;
                    }
                    continue;
                }

                // else, H_k
                int rAinv = inverse[rA];
                int rBinv = inverse[rB];
                int rCinv = inverse[rC];
                int Dk, Ek, Fk, Gk;
                
                // create whole Hk row at once
                while (col < right && col < width - row) {
                    Dk = get(row - 1, col);
                    Ek = get(top - 1, left + k);
                    Fk = get(top + k, left - 1);
                    Gk = get(top + l + 1 - k, right + 1);

                    
                    Ak = get(top, left + k);
                    Bk = get(top + k, left);
                    Ck = get(top + l + 1 - k, right);

                    int Hk = Dk * rCinv % modulo
                        * (Ek * rB * inverse[Ak] % modulo
                        + (1 - 2 * (k % 2)) * (Fk * rA * inverse[Bk] % modulo)
                        - (1 - 2 * (k % 2)) * (Gk * rD * inverse[Ck] % modulo));
                    set(row, col, (modulo + (Hk % modulo)) % modulo);

                    ++col;
                    --k;
                }

            } // while (col < width - row)
            ++row;
        } // while (row < height)
    }

    bool valid(int row, int col) {
        return (row < height && col >= row && col < width - row);
    }

    virtual void printWall() {
        for (int i = 0; i < width; ++i) {
            cout << " " << 0 << " ";
        }
        cout << "\n";
        for (int i = 0; i < width; ++i) {
            cout << " " << 1 << " ";
        }
        cout << "\n";
        for (int row = 0; row < height; ++row) {
            for (int i = 0; i < row; ++i) {
                cout << "   ";
            }
            for (int col = row; col < width - row ; ++col) {
                int curr = get(row, col);
                if (curr >= 0) cout << " ";
                cout << curr << " ";
            }
            cout << "\n";
        }
    }
    
    // save image of number wall
    virtual void savePNG(string filename, int pixelSize, int mod) {
        long long imgW = width * pixelSize;
        long long imgH = height * pixelSize;

        vector<unsigned char> img(imgW * imgH * 3);

        for (int row = 0; row < height; ++row) {

            for (int col = 0; col < width; ++col) {

                int r, g, b;

                if (!valid(row, col)) {
                    r = g = b = 255;
                } else {
                    int v = get(row, col);

                    int x = ((v % mod) + mod) % mod;

                    if (x == 0) {
                        r = 255;
                        g = 102;
                        b = 0;
                    }
                    else {
                        r = 0;
                        g = 0;
                        b = 50 + (205 * x) / (mod - 1);
                    }
                }

                for (int py = 0; py < pixelSize; ++py) {
                    for (int px = 0; px < pixelSize; ++px) {

                        long long X = col * pixelSize + px;
                        long long Y = row * pixelSize + py;

                        long long idx = 3 * (Y * imgW + X);

                        img[idx + 0] = r;
                        img[idx + 1] = g;
                        img[idx + 2] = b;
                    }
                }
            }
        }

        bool valid_write = stbi_write_png(
            filename.c_str(),
            imgW,
            imgH,
            3,
            img.data(),
            imgW * 3
        );
        if (!valid_write) cout << "ERROR! Failed to write image." << endl;
    }
    
    // save image of number wall
    virtual void savePNGSquare(string filename, int pixelSize, int mod) {
        bool pixelSizeIsOne = (pixelSize == 1);
        int colStart = 0;
        while (get(0, colStart) == 0) {
            ++colStart;
        }
        int colEnd = width - colStart;
        int sideLength = colEnd - colStart;

        long long imgW = sideLength * pixelSize;
        long long imgH = imgW; // assuming actually a square

        vector<unsigned char> img(imgW * imgH * 3);

        for (int row = 0; row < sideLength; ++row) {

            for (int col = colStart; col < colEnd; ++col) {

                int r, g, b;

                if (!valid(row, col)) {
                    r = g = b = 255;
                } else {
                    int v = get(row, col);

                    int x = ((v % mod) + mod) % mod;

                    if (x == 0) {
                        r = 255;
                        g = 102;
                        b = 0;
                    }
                    else {
                        r = 0;
                        g = 0;
                        b = 50 + (205 * x) / (mod - 1);
                    }
                }

                if (pixelSizeIsOne) { // faster if pixelSize is 1
                    long long idx = 3 * (row * imgW + (col - colStart));
                    img[idx + 0] = r;
                    img[idx + 1] = g;
                    img[idx + 2] = b;
                } else {
                    for (int py = 0; py < pixelSize; ++py) {
                        for (int px = 0; px < pixelSize; ++px) {

                            long long X = (col - colStart)  * pixelSize + px;
                            long long Y = row * pixelSize + py;

                            long long idx = 3 * (Y * imgW + X);

                            img[idx + 0] = r;
                            img[idx + 1] = g;
                            img[idx + 2] = b;
                        }
                    }
                }
            }
        }

        bool valid_write = stbi_write_png(
            filename.c_str(),
            imgW,
            imgH,
            3,
            img.data(),
            imgW * 3
        );
        if (!valid_write) cout << "ERROR! Failed to write image." << endl;
    }

    bool validWall() {
        for (int row = 1; row < height - 1; ++row) {
            for (int col = row + 1; col < width - row - 1; ++col) {
                if ((get(row - 1, col) * get(row + 1, col)
                    + get(row, col - 1) * get(row, col + 1)
                    - get(row, col) * get(row, col)) % modulo) {
                        return false;
                }
            }
        }
        return true;
    }
};



// to be added later
// struct NumberWallSquare { ... };
// struct NumberWallDet { ... };