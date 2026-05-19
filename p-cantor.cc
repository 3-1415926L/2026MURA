#include "NumberWallsV3.hpp"

#include <iostream>
#include <vector>
#include <utility>
#include <fstream>
#include <string>

using namespace std;

int chooseModP(int n, int k, int p) {
    if (k < 0 || k > n) {
        return 0;
    }
    k = min(k, n - k);

    int numerator = 1;
    int denominator = 1;

    for (int i = 1; i <= k; ++i) {
        numerator = numerator * (n - k + i) % p;
        denominator = denominator * i % p;
    }

    // Fermat inverse: denominator^(p-2) mod p
    int denominator_inverse = modPow(denominator, p - 2, p);

    return numerator * denominator_inverse % p;
}

void pCantor(vector<int>& S, int p) {
    int p2 = (p - 1) / 2;
    vector<int> binCoeff = {1};

    for (int i = 1; i <= p2; i++) {
        binCoeff.push_back(chooseModP(p2, i, p));
    }

    vector<int> result;
    result.reserve(S.size() * p);

    for (int n : S) {
        result.push_back(n);
        for (int i = 1; i <= p2; i++) {
            result.push_back(0);
            result.push_back((n * binCoeff[i]) % p);
        }
    }
    S = move(result);
}

vector<int> zeroPad(vector<int> S) {
    vector<int> result(S.size() * 3);
    copy(S.begin(), S.end(), result.begin() + S.size());
    return result;
}



int main() {
    vector<int> S = {1};
    int max_width = 1000;
    int pixel_size = 1;
    int modulo = 7;

    while (S.size() < max_width) {
        pCantor(S, modulo);
    }
    S = zeroPad(S);
    
    int len = S.size();
    NumberWall W{S, len, modulo};

    //for (int i = 0; i < len; ++i) {
    //    cout << S[i];
    //}

    //cout << "\n\nNW:\n";
    //W.printWall();

    //cout << "\n\n\n" << *max_element(W.wall.begin(), W.wall.end());

    string out_file = "pCantorNumberWalls/" + to_string(modulo)
                      + "-Cantor_(w=" + to_string(len) + ").png";
    W.savePNG(out_file, pixel_size, modulo);

    return 0;
}