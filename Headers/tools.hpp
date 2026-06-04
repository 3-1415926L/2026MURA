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

// computes the determinant of A, mod p = prime
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

void printArr(int S[], int len, ostream& out = cout) {
    if (len == 0) {
        out << "(empty)" << endl;
        return;
    } else {
        out << S[0];
    }
    for (int i = 1; i < len; i++) {
        out << "," << S[i];
    }
    out << endl;
}

void printArr(vector<int> S, ostream& out = cout) {
    if (S.size() == 0) {
        out << "(empty)" << endl;
        return;
    } else {
        out << S[0];
    }
    for (int i = 1; i < S.size(); i++) {
        out << "," << S[i];
    }
    out << endl;
}

// prints a 2D vector of anything printable
//   separated by spaces and newlines
template <typename T>
void printGrid(const vector<vector<T>> grid, ostream& out = cout) {
    for (auto& row : grid) {
        for (auto& element : row) {
            out << element << " ";
        }
        out << endl;
    }
    out << endl;
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