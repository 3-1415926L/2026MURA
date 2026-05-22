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



// standalone functions

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

// modular determinant
long long modpow(long long a, long long e, long long mod) {
    long long r = 1;
    while (e > 0) {
        if (e & 1) r = (r * a) % mod;
        a = (a * a) % mod;
        e >>= 1;
    }
    return r;
}

long long modinv(long long a, long long p) {
    return modpow((a % p + p) % p, p - 2, p);
}

long long det_mod(vector<vector<long long>> A, long long p) {
    int n = A.size();
    long long det = 1;
    int sign = 1;

    for (int i = 0; i < n; i++) {
        int pivot = i;
        while (pivot < n && A[pivot][i] % p == 0) pivot++;

        if (pivot == n) return 0;

        if (pivot != i) {
            swap(A[pivot], A[i]);
            sign = -sign;
        }

        long long piv = (A[i][i] % p + p) % p;
        det = (det * piv) % p;

        long long inv = modinv(piv, p);

        for (int j = i + 1; j < n; j++) {
            long long factor = (A[j][i] % p + p) % p;
            if (factor == 0) continue;

            long long mult = (factor * inv) % p;

            for (int k = i; k < n; k++) {
                A[j][k] = (A[j][k] - mult * A[i][k]) % p;
            }
        }
    }

    if (sign == -1) det = (p - det) % p;
    return det;
}

struct NumberWall {
    vector<int> wall; // does not include row -1 or -2
    vector<int> offset; // starting indices of each row
    int width;
    int height; // does not include row -1 or -2
    int modulo;
    vector<int> inverse; // used for precomputing modular inverses

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
        //makeWall(S);
        for(int i = 0; i < width; ++i) {
            wall[i] = (modulo + (S[i] % modulo)) % modulo;
        }
        // create other rows
        int row = 1, col = 1;
        while (row < height) {
            cout << "row=" << row << endl;
            col = row;
            while (col < width - row) {
                set(row, col, get_element(S, row, col));
                ++col;
            }
        ++row;
        }
    }

    // both of these require row <= col < width - row
    int get(int row, int col) {
        if (!valid(row, col)) {
            cout << "ERROR! get(" << row << "," << col << ") out of bounds!" << endl;
            abort();
        }
        if (row == -1) return 1;
        if (row < -1) return 0;
        return wall[offset[row] + col - row];
    }

    // Sets entries at row, col to value
    void set(int row, int col, int value) {
        if (!valid(row, col)) {
            cout << "ERROR! set(" << row << "," << col << ") out of bounds!" << endl;
            abort();
        }
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

                    int Hk = Dk * rCinv
                        * (Ek * rB * inverse[Ak]
                        + (1 - 2 * (k % 2)) * Fk * rA * inverse[Bk]
                        - (1 - 2 * (k % 2)) * Gk * rD * inverse[Ck]);
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
                } else {
                    int v = get(row, col);

                    int x = ((v % mod) + mod) % mod;

                    if (x == 0) {
                        r = 255;
                        g = b = 0;
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

    

    long long get_element(vector<int> S, int row, int col) {
        vector<vector<long long>> A(row + 1, vector<long long>(row + 1));
        for (int r = 0; r < row + 1; r++) {
            for (int c = 0; c < row + 1; c++) {
                int idx = col + (c - r);
                A[r][c] = S[idx];
            }
        }
        return det_mod(A, modulo);
    }
};



// Pagoda definition
vector<int> pagodaNext(vector<int> S) {
    vector<int> result;
    result.reserve(S.size() * 2);

    for (int n : S) {
        if (n == 1) {
            result.push_back(1);
            result.push_back(3);
        } else if (n == 2) {
            result.push_back(2);
            result.push_back(3);
        } else if (n == 3) {
            result.push_back(1);
            result.push_back(4);
        } else { // n = 4
            result.push_back(2);
            result.push_back(4);
        }
    }
    return result;
}

vector<int> pagodaCode(vector<int> S) {
    vector<int> result;
    result.reserve(S.size() * 4);

    for (int n : S) {
        if (n == 1) {
            result.push_back(-1);
            result.push_back(-1);
            result.push_back(0);
            result.push_back(1);
        } else if (n == 2) {
            result.push_back(0);
            result.push_back(-1);
            result.push_back(-1);
            result.push_back(1);
        } else if (n == 3) {
            result.push_back(0);
            result.push_back(-1);
            result.push_back(1);
            result.push_back(1);
        } else { // n = 4
            result.push_back(1);
            result.push_back(-1);
            result.push_back(0);
            result.push_back(1);
        }
    }
    return result;
}

vector<int> pagoda(int numTerms) {
    vector<int> S = {1};
    numTerms = numTerms / 4 + 1;
    while (S.size() < numTerms) {
        S = pagodaNext(S);
    }
    S = pagodaCode(S);
    S.erase(S.begin());
    return S;
}

int main() {
    int max_width = 400;
    vector<int> S = pagoda(max_width);
    int p = 79;

    NumberWall W(S, max_width, p);
    //W.printWall();
    W.savePNG("temp.png", 1, 79);

    cout << "\nvalid?=" << W.validWall();

    return 0;
}