#pragma once

#include "stb_image_write.h"
#include "tools.hpp"

#include <iostream>
#include <vector>
#include <utility>
#include <cmath>
#include <fstream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <type_traits>

using namespace std;



// wall storage types

struct FlatLayout {};
struct NestedLayout {};
struct FlatSquareLayout {};
struct NestedSquareLayout {};

struct Empty {};



// abstract base class for every type of Numberwall
template<typename Layout>
struct NumberWallBase {
    int width;
    int height; // does not include row -1 or -2
    int modulo;
    vector<int> inverse;

    vector<int> S; // stores the sequence
    
    // uses the specified data type for wall
    using wallType =
        conditional_t<
            is_same_v<Layout, FlatLayout> ||
            is_same_v<Layout, FlatSquareLayout>,
            vector<int>,
            vector<vector<int>>
        >;

    wallType wall;

    // creates offset vector iff using FlatLayout
    [[no_unique_address]]
    conditional_t<
        is_same_v<Layout, FlatLayout>,
        vector<int>,
        struct Empty
    > offset;

    // ctor
    NumberWallBase(vector<int>& S, int modulo): S{S},
            modulo{modulo}, width{static_cast<int>(S.size())} {

        if constexpr (is_same_v<Layout, FlatLayout> ||
                      is_same_v<Layout, NestedLayout>) {
            height = (width + 1) / 2;
        }
        else {
            height = width;
        }

        // get offset iff using FlatLayout
        if constexpr (is_same_v<Layout, FlatLayout>) {
            int total = 0, row = 0;
            for (int n = width; n >= 1; n -= 2) {
                offset.push_back(total - row);
                // get(row, col) = wall[offset[row] + col]
                row++;
                total += n;
            }
            wall.resize(total);
        }
        else if constexpr (is_same_v<Layout, NestedLayout>) {
            wall.resize(height);
            for (int i = 0; i < height; ++i) {
                wall[i] = vector<int>(width - 2 * i);
            }
        }
        else if constexpr (is_same_v<Layout, FlatSquareLayout>) {
            wall.resize(width * height);
        }
        else { // NestedSquareLayout
            wall.resize(height);
            for (int i = 0; i < height; ++i) {
                wall[i] = vector<int>(width);
            }
        }

        // get inverses
        for (int i = 0; i < modulo; ++i) {
            inverse.push_back(modPow(i, modulo - 2, modulo));
        }

        // subclasses must call makeWall(S);
    }

    // will be overrided in subclasses
    virtual void makeWall() = 0;

    int get(int row, int col) const {
        if (row == -1) return 1;
        if (row < -1) return 0;
        if constexpr (is_same_v<Layout, FlatLayout>) {
            return wall[offset[row] + col];
        }
        else if constexpr (is_same_v<Layout, NestedLayout>) {
            return wall[row][col - row];
        }
        else if constexpr (is_same_v<Layout, FlatSquareLayout>) {
            if (col < 0 || col >= width) return 0;
            return wall[row * width + col];
        }
        else { // NestedSquareLayout
            if (col < 0 || col >= width) return 0;
            return wall[row][col];
        }
    }

    void set(int row, int col, int value) {
        if constexpr (is_same_v<Layout, FlatLayout>) {
            wall[offset[row] + col] = value;
        }
        else if constexpr (is_same_v<Layout, NestedLayout>) {
            wall[row][col - row] = value;
        }
        else if constexpr (is_same_v<Layout, FlatSquareLayout>) {
            wall[row * width + col] = value;
        }
        else { // NestedSquareLayout
            wall[row][col] = value;
        }
    }

    // Sets entries at row, [colStart, colEnd) to value
    void setRange(int row, int colStart, int colEnd, int value) {
        while (colStart < colEnd) {
            set(row, colStart, value);
            ++colStart;
        }
    }

    bool inWall(int row, int col) {
        if constexpr (is_same_v<Layout, FlatLayout> ||
                      is_same_v<Layout, NestedLayout>) {
            return (row >= 0 && row < height &&
                col >= row && col < width - row);
        }
        else {
            return (row >= 0 && row < height &&
                col >= 0 && col < width);
        }
    }

    bool validPos(int row, int col) {
        if (row < 0) return true;
        if constexpr (is_same_v<Layout, FlatLayout> ||
                      is_same_v<Layout, NestedLayout>) {
            return (row < height &&
                col >= row && col < width - row);
        }
        else {
            return (row < height);
        }
    }

    void printWall() { //////////////////////// UPDATE LATER
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
                long long curr = get(row, col);
                if (curr >= 0) cout << " ";
                cout << curr << " ";
            }
            cout << "\n";
        }
    }

    int getMaxNum() {
        if constexpr (is_same_v<Layout, FlatLayout> ||
                        is_same_v<Layout, FlatSquareLayout>) {
            return *max_element(wall.begin(), wall.end());
        }
        else { // Nested
            int maxNum = 0;
            for (const auto& row : wall) {
                maxNum = max(maxNum, *max_element(row.begin(), row.end()));
            }
            return maxNum;
        }
    }
    
    // save image of number wall
    void savePNG(string filename, int pixelSize) {
        int imgW = width * pixelSize;
        int imgH = height * pixelSize;
        int maxNum;
        if (modulo > 0) {
            maxNum = modulo - 1;
        } else {
            maxNum = getMaxNum();
        }

        vector<unsigned char> img(imgW * imgH * 3);

        for (int row = 0; row < height; ++row) {

            for (int col = 0; col < width; ++col) {

                int r, g, b;

                if (!validPos(row, col)) {
                    r = g = b = 255;
                }
                else {
                    int x = get(row, col);

                    if (x == 0) {
                        r = 255;
                        g = 102;
                        b = 0;
                    }
                    else if (x == -1) {
                        r = g = b = 255;
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

    // checks if all positions pass the test of the first frame equation
    bool checkFC1() {
        int col, maxCol;
        for (int row = 1; row < height - 1; ++row) {
            
            // get start col and maxCol
            if constexpr (is_same_v<Layout, FlatLayout> ||
                        is_same_v<Layout, NestedLayout>) {
                col = row + 1;
                maxCol = this->width - row - 1;
            }
            else {
                col = 1;
                maxCol = this->width - 1;
            }

            while (col < maxCol) {
                if (validPos(row, col) &&
                    (get(row - 1, col) * get(row + 1, col)
                    + get(row, col - 1) * get(row, col + 1)
                    - get(row, col) * get(row, col))
                    % modulo) {
                        throw "test";
                        return false;
                }
                ++col;
            }
        }
        return true;
    }

    int maxZeroSize() {
        int maxSize = 0, currentSize = 0;
        int row = 0;
        int col, maxCol;

        while (row < height) {

            // get start col and maxCol
            if constexpr (is_same_v<Layout, FlatLayout> ||
                        is_same_v<Layout, NestedLayout>) {
                col = row;
                maxCol = this->width - row;
            }
            else {
                col = 0;
                maxCol = this->width;
            }

            while (col < maxCol) {
                currentSize = 0;

                while (col < maxCol && !get(row, col)) {
                    currentSize++;
                    col++;
                }
                if (currentSize > maxSize) {
                    maxSize = currentSize;
                }
                ++col;
                while (col < maxCol && get(row, col)) {
                    ++col;
                }
            }
            ++row;
        }
        return maxSize;
    }
};



// ====================================================



// Abstract base class for Numberwalls that generate
//   each of their elements independently
template<auto getElement, typename Layout = FlatLayout>
struct NumberWallByElement: public NumberWallBase<Layout> {

    // ctor
    NumberWallByElement(vector<int>& S, int modulo):
            NumberWallBase<Layout>{S, modulo} {
        makeWall();
    }

    virtual void makeWall() override {
        int row = 0;
        int col, maxCol;
        while (row < this->height) {

            // get start col and maxCol
            if constexpr (is_same_v<Layout, FlatLayout> ||
                        is_same_v<Layout, NestedLayout>) {
                col = row;
                maxCol = this->width - row;
            }
            else {
                col = 0;
                maxCol = this->width;
            }

            // main loop
            while (col < maxCol) {
                this->set(row, col, getElement(*this, row, col));
                ++col;
            }
            ++row;
        }
    }
};



// ====================================================



// NumberWallDet
constexpr auto getElementDet = []<typename NumberWallType>
    (NumberWallType& W, int row, int col) -> long long {

    vector<vector<long long>> A(row + 1, vector<long long>(row + 1));
    int len = W.S.size();

    for (int r = 0; r < row + 1; r++) {
        for (int c = 0; c < row + 1; c++) {
            int idx = col + (c - r);

            if (idx < 0 || idx >= len) {
                A[r][c] = 0;
            } else {
                A[r][c] = W.S[idx];
            }
        }
    }

    return detMod(A, W.modulo);
};

template<typename Layout = FlatLayout>
using NumberWallDet = NumberWallByElement<getElementDet, Layout>;



// NumberWallPerm
constexpr auto getElementPerm = []<typename NumberWallType>
    (NumberWallType& W, int row, int col) -> long long {

    vector<vector<long long>> A(row + 1, vector<long long>(row + 1));
    int len = W.S.size();

    for (int r = 0; r < row + 1; r++) {
        for (int c = 0; c < row + 1; c++) {
            int idx = col + (c - r);

            if (idx < 0 || idx >= len) {
                A[r][c] = 0;
            } else {
                A[r][c] = W.S[idx];
            }
        }
    }

    return permMod(A, W.modulo);
};

template<typename Layout = FlatLayout>
using NumberWallPerm = NumberWallByElement<getElementPerm, Layout>;



// NumberWallPermRyser
constexpr auto getElementPermRyser = []<typename NumberWallType>
    (NumberWallType& W, int row, int col) -> long long {

    vector<vector<long long>> A(row + 1, vector<long long>(row + 1));
    int len = W.S.size();

    for (int r = 0; r < row + 1; r++) {
        for (int c = 0; c < row + 1; c++) {
            int idx = col + (c - r);
            
            if (idx < 0 || idx >= len) {
                A[r][c] = 0;
            } else {
                A[r][c] = W.S[idx];
            }
        }
    }

    return permModRyser(A, W.modulo);
};

template<typename Layout = FlatLayout>
using NumberWallPermRyser = NumberWallByElement<getElementPermRyser, Layout>;



// NumberWallDet3D
constexpr auto getElementDet3D = []<typename NumberWallType>
    (NumberWallType& W, int row, int col) -> long long {
    // if not enough entries to fill in 3D tensor
    if (W.width - col <= 2 * row) {
        return -1;
    }

    vector<vector<vector<long long>>> A(row + 1,
        vector<vector<long long>>(row + 1,
            vector<long long>(row + 1)
        )
    );
    int len = W.S.size();

    for (int x = 0; x <= row; x++) {
        for (int y = 0; y <= row; y++) {
            for (int z = 0; z <= row; z++) {
                int idx = col + (y - x) + (z - x);
            
                if (idx < 0 || idx >= len) {
                    A[x][y][z] = 0;
                } else {
                    A[x][y][z] = W.S[idx];
                }
            }
        }
    }

    return det3DMod(A, W.modulo);
};

template<typename Layout = FlatLayout>
using NumberWallDet3D = NumberWallByElement<getElementDet3D, Layout>;



// ====================================================



template<typename Layout = FlatLayout>
struct NumberWall: public NumberWallBase<Layout> {

    // these are here so I don't have to preface each method/field from  with this->
    using NumberWallBase<Layout>::width;
    using NumberWallBase<Layout>::height;
    using NumberWallBase<Layout>::modulo;
    using NumberWallBase<Layout>::inverse;
    using NumberWallBase<Layout>::S;

    using NumberWallBase<Layout>::get;
    using NumberWallBase<Layout>::set;
    using NumberWallBase<Layout>::setRange;
    using NumberWallBase<Layout>::validPos;


    // ctor
    NumberWall(vector<int>& S, int modulo):
            NumberWallBase<Layout>{S, modulo} {
        makeWall();
    }

    virtual void makeWall() override {
        // zero/one rows implicitly there
        // create sequence row
        for(int i = 0; i < width; ++i) {
            set(0, i, (modulo + (S[i] % modulo)) % modulo);
        }
        // create other rows
        int row = 1;
        int col, maxCol;
        while (row < height) {

            // get start col and maxCol
            if constexpr (is_same_v<Layout, FlatLayout> ||
                        is_same_v<Layout, NestedLayout>) {
                col = row;
                maxCol = width - row;
            }
            else {
                col = 0;
                maxCol = width;
            }

            // main loop
            while (col < maxCol) {

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
                    while (validPos(top, left - 1) && get(top, left - 1) == 0) {
                        left--;
                    }
                }
                while (validPos(top, right + 1) && get(top, right + 1) == 0) {
                    right++;
                }
                top--;
                left--;
                right++;

                // if 0 square extends to edge, W[row,col] must be 0
                //   (only need to check the non-square cases, though)
                if constexpr (is_same_v<Layout, FlatLayout> ||
                            is_same_v<Layout, NestedLayout>) {
                    if (!(validPos(top,left) && validPos(top,right))) {
                        setRange(row, col, min(right, maxCol), 0);
                        col = right;
                        continue;
                    }
                }

                // if 0 square is wider than it is tall, W[row,col] must be 0
                if (right - left > row - top) {
                    setRange(row, col, min(right, maxCol), 0);
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
                    while (col <= right && col < maxCol) {
                        Dk = Dk * rDinv % modulo;
                        set(row, col, Dk);
                        ++col;
                    }
                    continue;
                }

                // else, H_k
                int rCinv = inverse[rC];
                int Dk, Ek, Fk, Gk;
                
                // create whole Hk row at once
                while (col < right && col < maxCol) {
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

            } // while (col < maxCol)
            ++row;
        } // while (row < height)
    }
}; // NumberWall



// ===============================================================



struct NumberWallNoMod {
    vector<long long> wall; // does not include row -1 or -2
    vector<int> offset;
    int width;
    int height; // does not include row -1 or -2

    // ctor
    NumberWallNoMod(vector<int>& S, bool print = 0):
            width{static_cast<int>(S.size())} {
        height = (width + 1) / 2;
        int total = 0;
        for (int n = width; n >= 1; n -= 2) {
            offset.push_back(total);
            total += n;
        }
        wall.resize(total);
        makeWall(S, print);
    }

    // both of these require row <= col < width - row
    long long get(int row, int col) {
        if (row >= 0 && (col < row || col >= width - row)) {
            cout << "error! " << row << ", " << col << " out of bounds!\n" << flush;
        }
        if (row == -1) return 1;
        if (row < -1) return 0;
        return wall[offset[row] + col - row];
    }

    void set(int row, int col, long long value) {
        wall[offset[row] + col - row] = value;
    }

    void makeWall(vector<int>& S, bool print) {
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
            wall[i] = S[i];
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
                long long curr = makeElement(row, col);
                if (print) {
                    if (curr >= 0) cout << " ";
                    cout << curr << " ";
                    cout << flush;
                }
                set(row, col, curr);
            }
            if (print) cout << "\n";
        }
    }

    bool valid(int row, int col) {
        return (row < height && col >= row && col < width - row);
    }

    long long makeElement(int row, int col) {
        if (get(row - 2, col) != 0) {
            return ((get(row - 1, col) * get(row - 1, col))
                - (get(row - 1, col - 1) * get(row - 1, col + 1)))
                / get(row - 2, col);
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
        long long Ak = get(top, left + k);
        long long Bk = get(top + k, left);
        long long Ck = get(top + l + 1 - k, right);

        // D_k
        if (get(row - 1, col) == 0) {
            long long Dk = pow(-1, l * k) * Bk * Ck / Ak;
            return Dk;
        }

        // else, H_k
        long long Dk = get(row - 1, col);

        long long rAnum = get(top, left + 1), rAden = get(top, left);
        long long rBnum = get(top + 1, left), rBden = get(top, left);
        long long rCnum = get(top, right), rCden = get(top + 1, right);
        long long rDnum = get(row - 1, col), rDden = get(row - 1, col + 1);

        long long Ek = get(top - 1, left + k);
        long long Fk = get(top + k, left - 1);
        long long Gk = get(top + l + 1 - k, right + 1);

        long long Hk = (Dk * rCden * Ek * rBnum / Ak / rBden
            + Dk * rCden * pow(-1, k) * Fk * rAnum / Bk / rAden
            - Dk * rCden * pow(-1, k) * Gk * rDnum / Ck / rDden) / rCnum;
        return Hk;
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
                long long curr = get(row, col);
                if (curr >= 0) cout << " ";
                cout << curr << " ";
            }
            cout << "\n";
        }
    }
    
    // save image of number wall
    void savePNG(string filename, int pixelSize, int mod = 0) {
        int imgW = width * pixelSize;
        int imgH = height * pixelSize;

        if (mod == 0) {
            mod = *max_element(wall.begin(), wall.end());
        }

        vector<unsigned char> img(imgW * imgH * 3);

        for (int row = 0; row < height; ++row) {

            for (int col = 0; col < width; ++col) {

                int r, g, b;

                if (!valid(row, col)) {
                    r = g = b = 255;
                }
                else {
                    long long v = get(row, col);

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

    int getMaxNum() {
        return *max_element(wall.begin(), wall.end());
    }

    bool checkFC1() {
        for (int row = 1; row < height - 1; ++row) {
            for (int col = row + 1; col < width - row - 1; ++col) {
                if (get(row - 1, col) * get(row + 1, col)
                    + get(row, col - 1) * get(row, col + 1)
                    - get(row, col) * get(row, col)) {
                        return false;
                }
            }
        }
        return true;
    }

    int maxZeroSize() {
        int maxSize = 0, currentSize = 0;
        int row = 0;
        int col;

        while (row < height) {
            col = row;
            while (col < width - row) {
                currentSize = 0;

                while (col < width - row && !get(row, col)) {
                    currentSize++;
                    col++;
                }
                if (currentSize > maxSize) {
                    maxSize = currentSize;
                }
                ++col;
                while (col < width - row && get(row, col)) {
                    ++col;
                }
            }
            ++row;
        }
        return maxSize;
    }
}; // NumberWallNoMod