#include <iostream>
#include <string>
#include <fstream>

using namespace std;

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

void lookAndSay(int numTerms, ostream& out) {
    string term = "1", S = "1";
    while (S.length() < numTerms) {
        term = lookAndSayNext(term);
        S = S + term;
    }
    for (int i = 0; i < S.length(); ++i) {
        out << i << " " << S[i] << endl;
    }
}

void lookAndSayAlt(int numTerms, ostream& out) {
    string S = "1";
    while (S.length() < numTerms) {
        S = S + lookAndSayNext(S);
    }
    for (int i = 0; i < S.length(); ++i) {
        out << i << " " << S[i] << endl;
    }
}

int main() {
    
    int numTerms = 10000;
    ofstream ff{"sequences/look_and_say_alt.txt"};
    lookAndSayAlt(numTerms, ff);

    return 0;
}