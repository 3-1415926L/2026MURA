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
    NumberWall(vector<int> S, int w, int modulo, bool print): width{w}, height{(w + 1) / 2}, modulo{modulo} {
        int total = 0;
        for (int n = w; n >= 1; n -= 2) {
            offset.push_back(total);
            total += n;
        }
        wall.resize(total);
        for (int i = 0; i < modulo; ++i) {
            inverse.push_back(pow(i, modulo - 2));
        }
        makeWall(S, print);
    }

    // both of these require row <= col < width - row
    int get(int row, int col) {
        if (row >= 0 && (col < row || col >= width - row)) {
            cout << "error! " << row << ", " << col << " out of bounds!\n" << flush;
        }
        if (row == -1) return 1;
        if (row < -1) return 0;
        return wall[offset[row] + col - row];
    }

    void set(int row, int col, int value) {
        wall[offset[row] + col - row] = value;
    }

    void makeWall(vector<int> S, bool print) {
        // zero/one rows implicitly there
        // if (print) print rows -2 and -1
        if (print) {
            cout << " ";
            for (int i = 0; i < width; ++i) {
                cout << 0 << "  ";
            }
            cout << "\n ";
            for (int i = 0; i < width; ++i) {
                cout << 1 << "  ";
            }
            cout << "\n";
        }
        // create sequence row
        for(int i = 0; i < width; ++i) {
            if (print) cout << " " << S[i] << " ";
            wall[i] = (modulo + (S[i] % modulo)) % modulo;
        }
        if (print) cout << "\n";
        // create other rows
        for (int row = 1; row < height; ++row) {
            if (print) {
                for (int i = 0; i < row; ++i) {
                    cout << "   ";
                }
            }
            for (int col = row; col < width - row ; ++col) {
                int curr = makeElement(row, col);
                if (print) {
                    cout << " " << curr << " " << flush;
                }
                set(row, col, curr);
            }
            if (print) cout << "\n";
        }
    }

    bool valid(int row, int col) {
        return (row < height && col >= row && col < width - row);
    }

    int makeElement(int row, int col) {
        if (get(row - 2, col) != 0) {
            int x = (((get(row - 1, col) * get(row - 1, col))
                - (get(row - 1, col - 1) * get(row - 1, col + 1)))
                * inverse[get(row - 2, col)]);
            return (modulo + (x % modulo)) % modulo;
        }

        // else D_k or H_k (i.e., zero square above)
        int top = row - 2, left = col, right = col;
        while (get(top - 1, col) == 0) {
            top--;
        }
        while (valid(top, left - 1) && get(top, left - 1) == 0) {
            left--;
        }
        while (valid(top, right + 1) && get(top, right + 1) == 0) {
            right++;
        }
        top--;
        left--;
        right++;

        // if 0 square extends to edge, W[row,col] must be 0
        if (!(valid(top,left) && valid(top,right))) return 0;

        // same here
        if (right - left > row - top) return 0;

        // else, D_k or H_k
        int l = right - left - 1;
        int k = right - col;
        int Ak = get(top, left + k);
        int Bk = get(top + k, left);
        int Ck = get(top + l + 1 - k, right);

        // D_k
        if (get(row - 1, col) == 0) {
            int Dk = pow(-1, l * k) * Bk * Ck * inverse[Ak];
            return (modulo + (Dk % modulo)) % modulo;
        }

        // else, H_k
        int Dk = get(row - 1, col);

        int rA = get(top, left + 1) * inverse[get(top, left)];
        int rB = get(top + 1, left) * inverse[get(top, left)];
        int rC = get(top, right) * inverse[get(top + 1, right)];
        int rD = get(row - 1, col) * inverse[get(row - 1, col + 1)];

        int Ek = get(top - 1, left + k);
        int Fk = get(top + k, left - 1);
        int Gk = get(top + l + 1 - k, right + 1);

        int Hk = Dk * inverse[rC % modulo]
            * (Ek * rB * inverse[Ak]
            + pow(-1, k) * Fk * rA * inverse[Bk]
            - pow(-1, k) * Gk * rD * inverse[Ck]);
        return (modulo + (Hk % modulo)) % modulo;
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
    NumberWall W{S, len, modulo, false};

    //W.printWall();

    string out_file = "images/" + sequence_file + "_(w=" + to_string(c) + ")_(m=" + to_string(modulo) + ").png";
    W.savePNG(out_file , pixel_size, modulo);
    //cout << "\n\n\n" << *max_element(W.wall.begin(), W.wall.end());

    return 0;
}