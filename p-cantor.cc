#include "NumberWallsV3.hpp"

#include <iostream>
#include <vector>
#include <utility>
#include <fstream>
#include <string>

using namespace std;



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

// uses (1+x^2+...+x^(2k))^n instead of (1+x^2)^p2
// note: p-Cantor sequence = (p,1)-Hare sequence
vector<int> pkHareCoeffs(int n, int k) {
    vector<int> coeffs(2 * k * n + 1);
    int c, sum;
    for (int i = 0; i <= k * n; ++i) { // includes first and last indices
        c = -1;
        sum = 0;
        for (int j = 0; j <= i / (k + 1); ++j) {
            c = 0 - c; // first term has c = 1
            sum += c * choose(n, j)
                   * choose(i - j * (k + 1) + n - 1, n - 1);
        }
        coeffs[2 * i] = sum; // odd indices already 0
    }
    return coeffs;
}

void pkHare(vector<int>& S, int p, int k) {
    if (!isPrime(p)) {
        cout << "Error! p=" << p << "is not prime" << endl;
        return;
    }
    int p2 = (p - 1) / 2;
    if (p2 % k != 0) {
        cout << "Error! (p-1)/2=" << p2 << " is not divisible by k=" << k << endl;
        return;
    }
    int n = p2 / k;
    vector<int> coeffs = pkHareCoeffs(n, k);

    vector<int> result;
    result.reserve(S.size() * p);

    for (int n : S) {
        for (int i = 0; i < p; i++) {
            result.push_back((n * coeffs[i]) % p);
        }
    }
    S = move(result);
}

vector<int> zeroPad(vector<int> S) {
    vector<int> result(S.size() * 3);
    copy(S.begin(), S.end(), result.begin() + S.size());
    return result;
}

vector<int> zeroPadLeft(vector<int> S) {
    vector<int> result(S.size() * 2);
    copy(S.begin(), S.end(), result.begin() + S.size());
    return result;
}



int main() {
    vector<int> S = {1};
    int max_width = 10000;
    int pixel_size = 1;
    int p = 5;
    int k = 2;
    //cin >> p >> k;

    while (S.size() < max_width / 3) {
        pkHare(S, p, k);
    }
    S.resize(15000);
    S = zeroPadLeft(S);
    
    int len = S.size();
    NumberWall W{S, len, p};

    //for (int i = 0; i < len; ++i) {
    //    cout << S[i];
    //}

    //cout << "\n\nNW:\n";
    //W.printWall();

    //cout << "\n\n\n" << *max_element(W.wall.begin(), W.wall.end());

    //string out_file = "pCantorNumberWalls/" + to_string(modulo)
    //                  + "-Cantor_(w=" + to_string(len) + ").png";
    string out_file = "pCantorNumberWalls/---(" + to_string(p) + ","
                      + to_string(k) + ")-Hare_(w=" + to_string(len) + ").png";

    W.savePNG(out_file, pixel_size, p);

    return 0;
}