#include "NumberWallsV3.hpp"

#include <iostream>
#include <vector>
#include <utility>
#include <fstream>
#include <string>
#include <cmath>

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

int main() {
    vector<int> S;
    ifstream ff{"sequences/pagoda.txt"};
    int max_width = 10000;
    int i = 0, x = 0, c = 0;
    while (ff >> x && c < max_width) {
        if (i) {
            S.push_back(x);
            ++c;
        }
        i = 1 - i;
    }

    int p = -1;
    while (true) {
        p += 4;
        cout << "p=" << p;
        if (!isPrime(p)) {
            cout << endl;
            continue;
        }
        cout << " (prime)" << endl;
        NumberWall W(S, max_width, p);
        vector<int> wall = W.wall;
        int len = wall.size();
        /*
        for (int i = 1; i < len; i += 2) {
            if (!(wall[i])) {
                if (!wall[i - 1] && !(find(W.offset.begin(), W.offset.end(), i) != W.offset.end())) {
                    cout << "p=" << p << ", i=" << i << endl;
                    break;
                }
                ++i;
                if (!wall[i] && !(find(W.offset.begin(), W.offset.end(), i) != W.offset.end())) {
                    cout << "p=" << p << ", i=" << i << endl;
                    break;
                }
            }
        }
        */
        for (int row = 0; row < W.height; ++row) {
            for (int col = row; col < W.width - row - 1; ++col) {
                if (!W.get(row, col) && !W.get(row, col + 1)) {
                    cout << "row=" << row << "col=" << col;
                    break;
                }
            }
        }
    }

    return 0;
}