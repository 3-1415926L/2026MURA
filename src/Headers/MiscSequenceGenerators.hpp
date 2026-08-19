#pragma once

#include "tools.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

void saveSeq(string S, string seqName) {
    ofstream ff{"sequences/" + seqName + ".txt"};
    for (int i = 0; i < S.length(); ++i) {
        ff << i << " " << S[i] << endl;
    }
}

void saveSeq(vector<int> S, string seqName) {
    ofstream ff{"sequences/" + seqName + ".txt"};
    for (int i = 0; i < S.size(); ++i) {
        ff << i << " " << S[i] << endl;
    }
}

string lookAndSayNext(const string& s) {
    string result;
    int n = s.size();
    int i = 0;

    while (i < n) {
        char digit = s[i];
        int count = 0;
        while (i < n && s[i] == digit) {
            ++count;
            ++i;
        }
        result += to_string(count);
        result += digit;
    }
    return result;
}

vector<int> lookAndSay(int numTerms) {
    string term = "1", S = "1";
    while (S.length() < numTerms) {
        term = lookAndSayNext(term);
        S = S + term;
    }
    return toVectorInt(S);
}

vector<int> lookAndSayAlt(int numTerms) {
    string S = "1";
    while (S.length() < numTerms) {
        S = S + lookAndSayNext(S);
    }
    return toVectorInt(S);
}

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

vector<int> pCantor(int p, int numIters) {
    int p2 = (p - 1) / 2;
    vector<int> binCoeff = {1};

    for (int i = 1; i <= p2; i++) {
        binCoeff.push_back(chooseModP(p2, i, p));
    }
    vector<int> S = {1};

    for (int i = 0; i < numIters; ++i) {
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
    return S;
}

vector<int> pkCantorCoeffs(int n, int k) {
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

// uses (1+x^2+...+x^(2k))^n instead of (1+x^2)^p2
// note: p-Cantor sequence = (p,1)-Cantor sequence
vector<int> pkCantor(int p, int k, int numIters) {
    vector<int> S = {1};
    if (!isPrime(p)) {
        cout << "Error! p=" << p << "is not prime" << endl;
        return S;
    }
    int p2 = (p - 1) / 2;
    if (p2 % k != 0) {
        cout << "Error! (p-1)/2=" << p2 << " is not divisible by k=" << k << endl;
        return S;
    }
    int n = p2 / k;
    vector<int> coeffs = pkCantorCoeffs(n, k);

    for (int i = 0; i < numIters; ++i) {
        vector<int> result;
        result.reserve(S.size() * p);

        for (int n : S) {
            for (int i = 0; i < p; i++) {
                result.push_back((n * coeffs[i]) % p);
            }
        }
        S = move(result);
    }
    return S;
}