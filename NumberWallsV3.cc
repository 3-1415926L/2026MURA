#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
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
    int get(int row, int col) {
        //if (row >= 0 && (col < row || col >= width - row)) {
        //    cout << "error! " << row << ", " << col << " out of bounds!\n" << flush;
        //}
        if (row == -1) return 1;
        if (row < -1) return 0;
        return wall[offset[row] + col - row];
    }

    // Sets entries at row, col to value
    void set(int row, int col, int value) {
        //if (value < 0 || value >= modulo) {
        //    cout << "error! " << row << ", " << col << " cannot be assigned " << value << "!\n" << flush;
        //}
        wall[offset[row] + col - row] = value;
    }

    // Sets entries at row, [colStart, colEnd) to value
    void setRange(int row, int colStart, int colEnd, int value) {
        while (colStart < colEnd) {
            wall[offset[row] + colStart - row] = value;
            ++colStart;
        }
    }

    void makeWall(vector<int> S) {
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
                int rDinv = (1 - 2 * (l % 2)) * rB * rC * inverse[rA];
                rDinv = (modulo + (rDinv % modulo)) % modulo;

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
                int rD = inverse[rDinv];
                int rAinv = inverse[rA];
                int rBinv = inverse[rB];
                int rCinv = inverse[rC];
                int Dk, Ek, Fk, Gk;
                int Akinv = inverse[Ak];
                int Bkinv = inverse[Bk];
                int Ckinv = inverse[Ck];
                
                // create whole Hk row at once
                while (col < right && col < width - row) {
                    Dk = get(row - 1, col);
                    Ek = get(top - 1, left + k);
                    Fk = get(top + k, left - 1);
                    Gk = get(top + l + 1 - k, right + 1);

                    int Hk = Dk * inverse[rC % modulo]
                        * (Ek * rB * Akinv
                        + (1 - 2 * (k % 2)) * Fk * rA * Bkinv
                        - (1 - 2 * (k % 2)) * Gk * rD * Ckinv);
                    set(row, col, (modulo + (Hk % modulo)) % modulo);

                    ++col;
                    --k;
                    Akinv = Akinv * rAinv % modulo;
                    Bkinv = Bkinv * rBinv % modulo;
                    Ckinv = Ckinv * rCinv % modulo;
                }

            } // while (col < width - row)
            ++row;
        } // while (row < height)
    }

    bool valid(int row, int col) {
        return (row < height && col >= row && col < width - row);
    }

    void printWall() {
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
    void savePNG(string filename, int pixelSize, int mod) {
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
                    int v = get(row, col);

                    int x = ((v % mod) + mod) % mod;

                    if (x == 0) {
                        r = g = b = 0;
                    }
                    else {
                        r = 0;
                        g = 0;
                        b = 50 + (205 * x) / (mod - 1);
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

    // efficiently finds x^p mod m
    // requires p >= 0, x >= 0
    int modPow(int x, int p, int m) {
        int res = 1;
        while (p > 0) {
            p--;
            res = (res * x) % m;
        }
        return res;
    }
};

// standalone functions

void printArr(int S[], int len) {
    for (int i = 0; i < len; i++) {
        cout << S[i];
    }
    cout << '\n';
}



int main() {
    vector<int> S;
    ifstream ff{"input.txt"};
    // temp is a pointless string so that I can have a nicer input file
    string sequence_file, temp;
    int max_width, pixel_size, modulo;
    ff >> temp >> sequence_file;
    ff >> temp >> max_width;
    ff >> temp >> pixel_size;
    ff >> temp >> modulo;
    string full_sequence_file = "sequences/" + sequence_file + ".txt";

    ff = ifstream{full_sequence_file};
    int i = 0, c = 0, x = 0;
    if (full_sequence_file.find("oeis") != std::string::npos) {
        while ((ff >> x) && c < max_width) {
            if (i) {
                S.push_back(x);
                ++c;
            }
            i = 1 - i;
        }
    } else {
        while (c < max_width) {
            S.push_back(x);
            ++c;
        }
    }
    
    int len = S.size();

    // last argument says whether to print it as it builds it
    NumberWall W{S, len, modulo};

    // uncomment line below to see text display of wall
    //W.printWall();

    string out_file = "images/" + sequence_file + "_(w=" + to_string(c) + ")_(m=" + to_string(modulo) + ").png";
    W.savePNG(out_file , pixel_size, modulo);
    //cout << "\n\n\n" << *max_element(W.wall.begin(), W.wall.end());

    return 0;
}