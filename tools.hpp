#pragma once

#include <iostream>
#include <vector>
#include <utility>
#include <cmath>
#include <string>
#include <algorithm>

using namespace std;



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

void printArr(int S[], int len) {
    if (len == 0) {
        cout << "(empty)" << endl;
        return;
    } else {
        cout << S[0];
    }
    for (int i = 1; i < len; i++) {
        cout << "," << S[i];
    }
    cout << endl;
}

void printArr(vector<int> S) {
    if (S.size() == 0) {
        cout << "(empty)" << endl;
        return;
    } else {
        cout << S[0];
    }
    for (int i = 1; i < S.size(); i++) {
        cout << "," << S[i];
    }
    cout << endl;
}

// requires n > 0
int choose(int n, int k) {
    if (k < 0 || k > n) return 0;
    if (k * 2 > n) k = n-k;
    if (k == 0) return 1;

    int result = n;
    for(int i = 2; i <= k; ++i ) {
        result *= (n-i+1);
        result /= i;
    }
    return result;
}

// requires n > 0, p = prime
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
    // Fermat's little theorem for inverse
    int denominator_inverse = modPow(denominator, p - 2, p);
    return numerator * denominator_inverse % p;
}