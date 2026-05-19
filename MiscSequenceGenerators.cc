#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

void printSeq(string S, string seqName) {
    ofstream ff{"sequences/" + seqName + ".txt"};
    for (int i = 0; i < S.length(); ++i) {
        ff << i << " " << S[i] << endl;
    }
}

void printSeq(vector<int> S, string seqName) {
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

void lookAndSay(int numTerms) {
    string term = "1", S = "1";
    while (S.length() < numTerms) {
        term = lookAndSayNext(term);
        S = S + term;
    }
    printSeq(S, "look_and_say");
}

void lookAndSayAlt(int numTerms) {
    string S = "1";
    while (S.length() < numTerms) {
        S = S + lookAndSayNext(S);
    }
    printSeq(S, "look_and_say_alt");
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

void pagoda(int numTerms) {
    vector<int> S = {1};
    numTerms = numTerms / 4 + 1;
    while (S.size() < numTerms) {
        S = pagodaNext(S);
    }
    S = pagodaCode(S);
    S.erase(S.begin());
    printSeq(S, "pagoda");
}

int main() {
    
    int numTerms = 10000;
    pagoda(numTerms);

    return 0;
}