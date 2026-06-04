#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "tools.hpp"

#include <iostream>
#include <vector>
#include <utility>
#include <fstream>
#include <string>
#include <cstdint>
#include <map>
#include <cmath>

using namespace std;



using Chunk = vector<uint8_t>;
using ChunkGrid = vector<vector<Chunk>>;
using Rule = vector<vector<string>>;
uint8_t zeroColour[3] = {255, 102, 0};
// orange = {255, 102, 0}
// blue = {0, 0, 255}

ChunkGrid loadImageChunks(const string& filename, int chunkSize) {
    int W, H;
    int C;
    unsigned char* data = stbi_load(filename.c_str(), &W, &H, &C, 3);

    if (!data) {
        throw std::runtime_error("stbi_load failed: " + filename);
    }

    int chunksX = W / chunkSize;
    int chunksY = H / chunkSize;
    ChunkGrid chunks(chunksY, vector<Chunk>(chunksX));

    for (int cy = 0; cy < chunksY; cy++) {
        for (int cx = 0; cx < chunksX; cx++) {

            Chunk &chunk = chunks[cy][cx];
            chunk.resize(chunkSize * chunkSize);
            for (int y = 0; y < chunkSize; y++) {
                for (int x = 0; x < chunkSize; x++) {

                    int globalX = cx * chunkSize + x;
                    int globalY = cy * chunkSize + y;
                    int idx = (globalY * W + globalX) * 3;

                    uint8_t nonZero =
                        (data[idx]     == zeroColour[0] &&
                         data[idx + 1] == zeroColour[1] &&
                         data[idx + 2 ] == zeroColour[2])
                        ? 0 : 1;

                    chunk[y * chunkSize + x] = nonZero;
                }
            }
        }
    }

    stbi_image_free(data);
    return chunks;
}

int chunkWidth(Chunk chunk){
    return static_cast<int>(sqrt(chunk.size()));
}

// rotates 90 degrees clockwise
Chunk rotate(Chunk chunk) {
    int w = chunkWidth(chunk);
    Chunk result(chunk.size());
    for (int y = 0; y < w; y++) {
        for (int x = 0; x < w; x++) {
            result[x * w + (w - 1 - y)] = chunk[y * w + x];
        }
    }
    return result;
}

// rotates 180 degrees
Chunk rotate180(Chunk chunk) {
    int w = chunkWidth(chunk);
    int s = chunk.size();
    Chunk result(chunk.size());
    for (int y = 0; y < w; y++) {
        for (int x = 0; x < w; x++) {
            result[(s - 1) - (y * w + x)] = chunk[y * w + x];
        }
    }
    return result;
}

// rotates 90 degrees counter-clockwise
Chunk rotateBack(Chunk chunk) {
    int w = chunkWidth(chunk);
    Chunk result(chunk.size());
    for (int y = 0; y < w; y++) {
        for (int x = 0; x < w; x++) {
            result[(w - 1 - x) * w + y] = chunk[y * w + x];
        }
    }
    return result;
}

// flips top and bottom
Chunk reflectV(Chunk chunk) {
    int w = chunkWidth(chunk);
    Chunk result(chunk.size());
    for (int y = 0; y < w; y++) {
        for (int x = 0; x < w; x++) {
            result[(w - 1 - y) * w + x] = chunk[y * w + x];
        }
    }
    return result;
}

// flips left and right
Chunk reflectH(Chunk chunk) {
    int w = chunkWidth(chunk);
    Chunk result(chunk.size());
    for (int y = 0; y < w; y++) {
        for (int x = 0; x < w; x++) {
            result[y * w + (w - 1 - x)] = chunk[y * w + x];
        }
    }
    return result;
}

// rotates 90 degrees clockwise
string rotate(string symbol) {
    string base(1, symbol[0]);
    string suffix = symbol.substr(1);

    for (char& c : suffix) {
        if (c == 'N') c = 'E';
        else if (c == 'E') c = 'S';
        else if (c == 'S') c = 'W';
        else if (c == 'W') c = 'N';
        
        else if (c == 'V') c = 'H';
        else if (c == 'H') c = 'V';

        else if (c == 'P') c = 'R';
        else if (c == 'R') c = 'P';
        else if (c == 'Q') c = 'T';
        else if (c == 'T') c = 'Q';
    }
    return base + suffix;
}

// flips left and right
string reflectH(string symbol) {
    string base(1, symbol[0]);
    string suffix = symbol.substr(1);

    for (char& c : suffix) {
        if (c == 'E') c = 'W';
        else if (c == 'W') c = 'E';
        
        else if (c == 'C') c = 'A';
        else if (c == 'A') c = 'C';

        else if (c == 'P') c = 'Q';
        else if (c == 'Q') c = 'P';
        else if (c == 'R') c = 'T';
        else if (c == 'T') c = 'R';
    }
    return base + suffix;
}



// In the case of (5,2)-Cantor, units are 25 x 25 grids
//   that can uniquely determine what symbol you have
struct Unit {
    char type;
    int index;
    string canonicalVariant;
    // "" => all symmetries => 1 variant (canonical = only one)
    // "C" => 90 deg rotational symmetry => 2 variants ("A")
    // "P" => 180 deg rotational symmetry => 4 variants ("Q", "R", "T")
    //        (Q = refl(P), R = rot(P), T = rot(refl(P)) = refl(rot(P)))
    // "V" => 2 reflectional symmetries => 2 variants ("H") 
    // "N" => 1 reflectional symmetry => 4 variants ("E", "S", "W")
    // "NE" => no symmetry => 8 variants ("ES", "SW", "WN", "SE", "WS", "NW", "EN")
    Chunk identifier;
    int posX, posY;

    Unit(char type, int index, string canVar, Chunk chunk, int row, int col):
        type{type}, index{index}, canonicalVariant{canVar},
        identifier{chunk}, posY{row}, posX{col} {}

    bool operator==(Chunk& chunk) {
        return (identifier == chunk);
    }

    // check equality up to rotation/reflection
    bool isEquivalent(Chunk& chunk) {
        if (identifier == chunk
            || identifier == reflectH(chunk)
            || identifier == reflectV(chunk)
            || identifier == rotate(chunk)) return true;
        Chunk chunkCopy = rotateBack(chunk);
        if (identifier == chunkCopy
            || identifier == reflectH(chunkCopy)
            || identifier == reflectV(chunkCopy)
            || identifier == rotateBack(chunkCopy)) return true;
        return false;
    }

    string getSymbol() {
        if (type == '0') return "0";
        return type + to_string(index) + canonicalVariant;
    }

    int getPosX() {return posX;}
    int getPosY() {return posY;}
};

struct Morphism {
    int morphismSize; // ex. 0 -> {{0,0},{0,0}} would be size 2
    vector<Unit> units;
    vector<vector<uint8_t>> types; // not a vector<Chunk> unless minUniqueIter == 1
    vector<int> typeCounts; // 0 symbol not counted
    map<string, Rule> canonicalRules;

    Morphism(string imgFile, int morphismSize, int minUniqueIter):
                                        morphismSize{morphismSize} {
        ChunkGrid grid = loadImageChunks(imgFile, pow(morphismSize, minUniqueIter));

        findMorphism(grid);
    }

    void findMorphism(ChunkGrid& grid){
        int sideLength = grid.size() / morphismSize;

        // add 0 unit
        //int side = pow(morphismSize, minUniqueIter);
        Chunk zeroChunk(grid[0][0].size(), 0);
        units.emplace_back('0', 0, "", zeroChunk, 0, 0);

        // add all the other units
        for (int row = 0; row < sideLength; ++row) {
            for (int col = 0; col < sideLength; ++col) {
                addUnit(grid[row][col], row, col);
            }
        }

        // create rules map
        for (auto& u : units) {
            string symbol = u.getSymbol();
            // special case if zero
            if (symbol == "0") {
                canonicalRules["0"] = Rule(morphismSize,
                    vector<string>(morphismSize, "0"));
                continue;
            }

            // else
            int posX = u.getPosX();
            int posY = u.getPosY();
            canonicalRules[symbol] = {};
            for (int row = 0; row < morphismSize; ++row) {
                canonicalRules[symbol].push_back({});
                for (int col = 0; col < morphismSize; ++col) {

                    Chunk chunk = grid[posY * morphismSize + row][posX * morphismSize + col];
                    string chunkSymbol = getSymbol(chunk);
                    canonicalRules[symbol][row].push_back(chunkSymbol);
                }
            }
        }
    }

    void addUnit(Chunk& chunk, int row, int col) {
        if (inMorphism(chunk)) return;

        char type = getType(chunk);
        int typeIdx = type - 'A';
        int unitIdx = typeCounts[typeIdx];
        string canVar = getCanonicalVariant(chunk);

        units.emplace_back(type, unitIdx, canVar, chunk, row, col);
    }

    bool inMorphism(Chunk& chunk) {
        for (auto& u : units) {
            if (u.isEquivalent(chunk)) return true;
        }
        return false;
    }

    // assumes chunk is not all zeros
    char getType(Chunk& chunk) {
        vector<uint8_t> type;

        int sideLength = sqrt(chunk.size());
        int S = sideLength / morphismSize;
        int index;

        for (int R = 0; R < morphismSize; ++R) {
            for (int C = 0; C < morphismSize; ++C) {

                index = R * sideLength * S + C * S;
                bool allZero = 1;
                for (int i = 0; i < S; ++i) {
                    for (int j = 0; j < S; ++j) {

                        if (chunk[index + i * sideLength + j]) {
                            allZero = 0;
                            break;
                        }
                    }
                    if (!allZero) break;
                }
                if (allZero) type.push_back(0);
                else type.push_back(1);
            }
        }

        auto typeIt = findType(type, chunk);
        if (typeIt != types.end()) { // return existing type
            int index = typeIt - types.begin();
            typeCounts[index]++;
            return 'A' + index;
        } else { // create new type
            char newType = 'A' + types.size();
            typeCounts.push_back(1);
            types.push_back(type);
            return newType;
        }
    }

    vector<vector<uint8_t>>::iterator findType(vector<uint8_t>& type, Chunk& chunk) {
        for (auto it = types.begin(); it != types.end(); ++it) {
            if (*it == type) return it;
            type = rotate(type);
            if (*it == type) {
                return it;
            }
            type = rotate(type);
            if (*it == type) {
                return it;
            }
            type = rotate(type);
            if (*it == type) {
                return it;
            }
            type = rotate(type);
            type = reflectH(type);
            // again, but reflected
            if (*it == type) {
                return it;
            }
            type = rotate(type);
            if (*it == type) {
                return it;
            }
            type = rotate(type);
            if (*it == type) {
                return it;
            }
            type = rotate(type);
            if (*it == type) {
                return it;
            }
        }
        return types.end();
    }

    string getCanonicalVariant(Chunk& chunk) {
        bool r = (chunk == rotate(chunk));
        bool h = (chunk == reflectH(chunk));
        if (r && h) return "";
        if (r) return "C";
        bool r180 = (chunk == rotate180(chunk));
        if (r180 && h) return "V";
        if (r180) return "P";
        if (h) return "N";
        bool v = (chunk == reflectV(chunk));
        if (v) return "E";
        return "NE";
    }

    string getSymbol(Chunk chunk) { // not by reference
        string symbol;
        Chunk originalChunk = chunk;
        for (auto& u : units) {
            symbol = u.getSymbol();
            chunk = originalChunk;
            for (int i = 0; i < 4; ++i) {
                if (u == chunk) return symbol;
                chunk = rotateBack(chunk); // rotateBack
                symbol = rotate(symbol);
            }
            chunk = reflectH(chunk);
            // again, but reflected
            for (int i = 0; i < 4; ++i) {
                if (u == chunk) return reflectH(symbol);
                chunk = rotateBack(chunk); // rotateBack
                symbol = rotate(symbol);
            }
        }
        return "[Not Found]";
    }

    string getSuffix(const string& symbol) const {
        if (symbol == "0")
            return "";
        int pos = 1; // skip first letter
        while (pos < symbol.size() && isdigit(symbol[pos])) {
            ++pos;
        }
        return symbol.substr(pos);
    }

    void printCanonicalMorphism(ostream& out = cout) {
        for (auto& [symbol, rule] : canonicalRules) {
            out << symbol << ":" << endl;
            printGrid(rule, out);
        }
    }

    void printMorphism(ostream& out = cout) {
        for (auto& [originalSymbol, originalRule] : canonicalRules) {
            out << originalSymbol << endl;
            printGrid(originalRule, out);
            string suffix = getSuffix(originalSymbol);

            if (suffix == "") continue;

            string symbol = originalSymbol;
            Rule rule = originalRule;

            if (suffix == "C") {
                symbol = reflectH(symbol);
                rule = reflectHRule(rule);
                out << symbol << endl;
                printGrid(rule, out);
                continue;
            }

            if (suffix == "V") {
                symbol = rotate(symbol);
                rule = rotateRule(rule);
                out << symbol << endl;
                printGrid(rule, out);
                continue;
            }

            if (suffix == "P") {
                symbol = reflectH(symbol); // Q
                rule = reflectHRule(rule);
                out << symbol << endl;
                printGrid(rule, out);

                symbol = rotate(symbol); // R
                rule = rotateRule(rule);
                out << symbol << endl;
                printGrid(rule, out);

                symbol = reflectH(symbol); // S
                rule = reflectHRule(rule);
                out << symbol << endl;
                printGrid(rule, out);

                continue;
            }

            if (suffix == "N" || suffix == "E") {
                for (int i = 0; i < 3; ++i) {
                    symbol = rotate(symbol);
                    rule = rotateRule(rule);
                    out << symbol << endl;
                    printGrid(rule, out);
                }
                continue;
            }

            if (suffix == "NE") {
                for (int i = 0; i < 3; ++i) {
                    symbol = rotate(symbol);
                    rule = rotateRule(rule);
                    out << symbol << endl;
                    printGrid(rule, out);
                }

                symbol = rotate(symbol);
                rule = rotateRule(rule);
                symbol = reflectH(symbol);
                rule = reflectHRule(rule);
                out << symbol << endl;
                printGrid(rule, out);

                for (int i = 0; i < 3; ++i) { // again but reflected
                    symbol = rotate(symbol);
                    rule = rotateRule(rule);
                    out << symbol << endl;
                    printGrid(rule, out);
                }
                continue;
            }

            cerr << "Unknown suffix: " << suffix << endl;
        }
    }

    void printCoding(ostream& out = cout) {
        for (auto& [originalSymbol, originalRule] : canonicalRules) {
            if (originalSymbol == "0") {
                out << "0 0" << endl << endl;
                continue;
            }

            out << originalSymbol << " 1" << endl << endl;
            string suffix = getSuffix(originalSymbol);

            if (suffix == "") continue;

            string symbol = originalSymbol;

            if (suffix == "C") {
                symbol = reflectH(symbol);
                out << symbol << " 1" << endl << endl;
                continue;
            }

            if (suffix == "V") {
                symbol = rotate(symbol);
                out << symbol << " 1" << endl << endl;
                continue;
            }

            if (suffix == "P") {
                symbol = reflectH(symbol); // Q
                out << symbol << " 1" << endl << endl;

                symbol = rotate(symbol); // R
                out << symbol << " 1" << endl << endl;

                symbol = reflectH(symbol); // S
                out << symbol << " 1" << endl << endl;

                continue;
            }

            if (suffix == "N" || suffix == "E") {
                for (int i = 0; i < 3; ++i) {
                    symbol = rotate(symbol);
                    out << symbol << " 1" << endl << endl;
                }
                continue;
            }

            if (suffix == "NE") {
                for (int i = 0; i < 3; ++i) {
                    symbol = rotate(symbol);
                    out << symbol << " 1" << endl << endl;
                }

                symbol = rotate(symbol);
                symbol = reflectH(symbol);
                out << symbol << " 1" << endl << endl;

                for (int i = 0; i < 3; ++i) { // again but reflected
                    symbol = rotate(symbol);
                    out << symbol << " 1" << endl << endl;
                }
                continue;
            }
        }
    }

    // rotate 90 degrees (and rotate symbols inside)
    Rule rotateRule(Rule& rule) {
        int h = rule.size();
        int w = rule[0].size();
        Rule result(h, vector<string>(w));
        for (int r = 0; r < h; r++) {
            for (int c = 0; c < w; c++) {
                result[c][h - 1 - r] = rotate(rule[r][c]);
            }
        }
        return result;
    }

    // flips left and right (and flips symbols inside)
    Rule reflectHRule(Rule& rule) {
        int h = rule.size();
        int w = rule[0].size();
        Rule result(h, vector<string>(w));
        for (int r = 0; r < h; r++) {
            for (int c = 0; c < w; c++) {
                result[r][w - 1 - c] = reflectH(rule[r][c]);
            }
        }
        return result;
    }

    int countSymbols() {
        int count = 0;
        for (auto& [originalSymbol, originalRule] : canonicalRules) {
            string suffix = getSuffix(originalSymbol);

            if (suffix == "") count++;
            else if (suffix == "C" || suffix == "V") count += 2;
            else if (suffix == "P" || suffix == "N" || suffix == "E") count += 4;
            else if (suffix == "NE") count += 8;
            else cerr << "Unknown suffix: " << suffix << endl;
        }
        return count;
    }
};



int main() {

    int morphismSize = 5;
    int minUniqueIter = 2; // need this many iterations to uniquely determine what symbol you're looking at
    string imgFile = R"(imagesAutomaticSequences\3-cantor(auto).png)";

    //imgFile = R"(imagesAutomaticSequences\i=6,s=0,0-{{0,0,0},{0,1,0},{0,0,0}},1-{{1,1,1},{1,1,1},{1,1,1}}.png)";
    //imgFile = R"(imagesAutomaticSequences\a.png)";
    imgFile = R"(imagesAutomaticSequences\(5,2)-Hare(auto)_(i=6).png)";
    //imgFile = R"(imagesAutomaticSequences\5-cantor(auto).png)";
    //imgFile = R"(output.png)";



    Morphism M{imgFile, morphismSize, minUniqueIter};

    M.printCanonicalMorphism();

    cout << "===========================" << endl;
    
    //ofstream ff{"temp.txt"};
    M.printMorphism();
    M.printCoding();

    cout << "Canon len = " << M.canonicalRules.size() << endl;
    cout << "Full len = " << M.countSymbols() << endl;

    //int side = pow(morphismSize, minUniqueIter);
    //Chunk zeroChunk(side * side, 0);
    //cout << M.getSymbol(zeroChunk);

    return 0;
}