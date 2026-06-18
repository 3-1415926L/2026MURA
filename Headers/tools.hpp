#pragma once

#include <iostream>
#include <vector>
#include <utility>
#include <cmath>
#include <string>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <chrono>

using namespace std;



bool isPrime(int n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;

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
long long detMod(vector<vector<long long>> A, long long p) {
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

long long permMod(vector<vector<long long>> A, long long p) {
    int n = (int)A.size();
    long long ans = 0;

    vector<int> perm(n);
    for (int i = 0; i < n; i++)
        perm[i] = i;

    while (true) {
        long long term = 1;
        for (int i = 0; i < n; i++) {
            term = term * A[i][perm[i]] % p;
        }

        ans += term;
        ans %= p;

        if (!next_permutation(perm.begin(), perm.end())) break;
    }

    return ans;
}

// same as permMod, but using Ryser's formula
long long permModRyser(vector<vector<long long>> A, long long p) {
    int n = (int)A.size();

    long long N = 1LL << n;

    vector<long long> rowSum(n, 0);

    auto addmod = [&](long long a, long long b) {
        a += b;
        a %= p;
        if (a < 0) a += p;
        return a;
    };

    long long ans = 0;
    long long prevGray = 0;

    for (long long k = 1; k < N; ++k) {
        long long gray = k ^ (k >> 1);
        long long diff = gray ^ prevGray;

        int bit = __builtin_ctzll(diff);
        bool added = (gray >> bit) & 1;

        for (int i = 0; i < n; ++i) {
            if (added)
                rowSum[i] = addmod(rowSum[i], A[i][bit]);
            else
                rowSum[i] = addmod(rowSum[i], -A[i][bit]);
        }

        long long prod = 1;
        for (int i = 0; i < n; ++i)
            prod = (long long)prod * rowSum[i] % p;

        int parity = __builtin_popcountll(gray);

        if (((n - parity) & 1) == 0)
            ans = addmod(ans, prod);
        else
            ans = addmod(ans, -prod);

        prevGray = gray;
    }

    return ans;
}

// uses the permutation definition of the derivative, generalized to 3D.
long long det3DMod(const vector<vector<vector<long long>>>& A, long long p) {
    int n = A.size();

    if (n == 1)
        return A[0][0][0] % p;

    long long ans = 0;

    for (int y = 0; y < n; y++) {
        for (int z = 0; z < n; z++) {

            long long sign = ((y + z) % 2 == 0) ? 1 : -1;

            std::vector<std::vector<std::vector<long long>>> B;

            for (int x2 = 1; x2 < n; x2++) {
                std::vector<std::vector<long long>> plane;

                for (int y2 = 0; y2 < n; y2++) {
                    if (y2 == y) continue;

                    std::vector<long long> row;

                    for (int z2 = 0; z2 < n; z2++) {
                        if (z2 == z) continue;

                        row.push_back(A[x2][y2][z2] % p);
                    }

                    plane.push_back(row);
                }

                B.push_back(plane);
            }

            long long sub = det3DMod(B, p);

            long long term = A[0][y][z] % p;
            term = (long long)term * sub % p;
            term = (long long)term * sign % p;

            ans += term;
            ans %= p;
        }
    }

    if (ans < 0) ans += p;
    return ans;
}

// adds S.size() zeroes to the left and right of S
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

void printArr(const int S[], int len, ostream& out = cout) {
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

void printArr(const vector<int>& S, ostream& out = cout) {
    if (S.size() == 0) {
        out << "(empty)" << endl;
        return;
    } else {
        out << S[0];
    }
    for (size_t i = 1; i < S.size(); i++) {
        out << "," << S[i];
    }
    out << endl;
}

// prints a 2D vector of anything printable
//   separated by spaces and newlines
template <typename T>
void printGrid(const vector<vector<T>>& grid, ostream& out = cout) {
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

vector<int> toVectorInt(const string& S) {
    vector<int> result;
    result.reserve(S.size());

    for (char c : S) {
        result.push_back(c - '0');
    }

    return result;
}

template<typename T, typename U>
unordered_map<T, U> toUnorderedMap(const map<T, U>& M) {
    unordered_map<T, U> UM;
    for (auto& [key, value] : M) {
        UM[key] = value;
    }
    return UM;
}

template<typename T, typename U>
map<T, U> toMap(const unordered_map<T, U>& UM) {
    map<T, U> M;
    for (auto& [key, value] : UM) {
        M[key] = value;
    }
    return M;
}

// requires some numerical type
template<typename T, typename U>
bool compareZeros(const vector<vector<T>>& A, const vector<vector<U>>& B) {
    if (A.size() != B.size())
        return false;

    for (int i = 0; i < A.size(); ++i) {
        if (B[i].size() != B[i].size()) return false;

        for (int j = 0; j < A[i].size(); ++j) {
            if ((A[i][j] == 0) != (B[i][j] == 0)) return false;
        }
    }
    return true;
}

using Clock = chrono::steady_clock;
class StopWatch {
    Clock::time_point last;
public:
    StopWatch(): last{Clock::now()} {}
    void printAndReset(ostream& out = cout) {
        auto now = Clock::now();
        out << chrono::duration<double>(now - last).count() << "s";
        last = now;
    }
};

ostream& operator<<(ostream& out, StopWatch& SW) {
    SW.printAndReset(out);
    return out;
}

struct PairHash {
    size_t operator()(const pair<int,int>& p) const {
        return hash<int>{}(p.first) ^ (hash<int>{}(p.second) << 1);
    }
};