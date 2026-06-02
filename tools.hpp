#pragma once

#include <iostream>
#include <vector>
#include <utility>
#include <cmath>
#include <string>
#include <algorithm>

using namespace std;



bool isPrime(int n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;

    // Check for factors from 3 up to sqrt(n)
    int r = sqrt(n);
    for (int i = 3; i <= r; i += 2) {
        if (n % i == 0) {
            return false;
        }
    }
    return true; // No factors found -> prime
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

// same, but for 'long long' numbers
long long modpow(long long a, long long e, long long m) {
    long long r = 1;
    while (e > 0) {
        if (e & 1) r = (r * a) % m;
        a = (a * a) % m;
        e >>= 1;
    }
    return r;
}

long long modinv(long long a, long long p) {
    return modpow((a % p + p) % p, p - 2, p);
}

void printArr(int S[], int len, ostream& out = cout) {
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

void printArr(vector<int> S, ostream& out = cout) {
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

// prints a 2D vector of anything printable
//   separated by spaces and newlines
template <typename T>
void printGrid(const vector<vector<T>> grid, ostream& out = cout) {
    for (auto& row : grid) {
        for (auto& element : row) {
            cout << element << " ";
        }
        cout << endl;
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