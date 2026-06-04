#include "NumberWalls.hpp"

#include <iostream>
#include <vector>
#include <utility>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;



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