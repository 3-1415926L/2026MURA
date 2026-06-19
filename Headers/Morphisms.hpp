#pragma once

#include "stb_image.h"
#include "tools.hpp"

#include <iostream>
#include <vector>
#include <utility>
#include <fstream>
#include <string>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <cmath>
#include <utility>

using namespace std;



using Chunk = vector<uint8_t>;
using ChunkGrid = vector<vector<Chunk>>;
using Rule = vector<vector<string>>;
uint8_t defaultZeroColour[3] = {255, 102, 0};
// orange = {255, 102, 0}
// blue = {0, 0, 255}
// black = {0, 0, 0}

// from a file
ChunkGrid loadImageChunks(string& filename, int chunkSize, uint8_t zeroColour[3] = defaultZeroColour) {
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

// from a 2D vector
template<typename T>
ChunkGrid loadWallChunks(vector<vector<T>>& wall, int chunkSize) {
    int H = wall.size();
    int W = wall[0].size();

    int chunksX = W / chunkSize;
    int chunksY = H / chunkSize;
    ChunkGrid chunks(chunksY, vector<Chunk>(chunksX));

    for (int cy = 0; cy < chunksY; ++cy) {
        for (int cx = 0; cx < chunksX; ++cx) {

            Chunk& chunk = chunks[cy][cx];
            chunk.resize(chunkSize * chunkSize);
            for (int y = 0; y < chunkSize; ++y) {
                for (int x = 0; x < chunkSize; ++x) {

                    int globalX = cx * chunkSize + x;
                    int globalY = cy * chunkSize + y;

                    chunk[y * chunkSize + x] = (wall[globalY][globalX] == 0) ? 0 : 1;
                }
            }
        }
    }

    return chunks;
}



// from a 1D vector (assumes flattened square)
template<typename T>
ChunkGrid loadWallChunks(vector<T>& wall, int chunkSize) {
    int H = sqrt(wall.size());
    int W = H;

    

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

                    chunk[y * chunkSize + x] = (wall[globalY * W + globalX] == 0) ? 0 : 1;;
                }
            }
        }
    }

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

    // ctor
    Morphism(string& imgFile, int morphismSize, int minUniqueIter):
                                        morphismSize{morphismSize} {
        ChunkGrid grid = loadImageChunks(imgFile, pow(morphismSize, minUniqueIter));
        findMorphism(grid);
    }

    // ctor that can accept a Nested NumberWallSquare's wall
    template <typename T>
    Morphism(vector<vector<T>>& wall, int morphismSize, int minUniqueIter):
                                        morphismSize{morphismSize} {
        ChunkGrid grid = loadWallChunks(wall, pow(morphismSize, minUniqueIter));
        findMorphism(grid);
    }

    // ctor that can accept a Flat NumberWallSquare's wall
    template <typename T>
    Morphism(vector<T>& wall, int morphismSize, int minUniqueIter):
                                        morphismSize{morphismSize} {
        ChunkGrid grid = loadWallChunks(wall, pow(morphismSize, minUniqueIter));
        findMorphism(grid);
    }

    // skeleton ctor used for derived SquareWallMorphism
    Morphism(int morphismSize): morphismSize{morphismSize} {}

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

        auto typeIt = findType(type);
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

    vector<vector<uint8_t>>::iterator findType(vector<uint8_t>& type) {
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
        throw "Rule Not Found";
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
            out << symbol << endl;
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

    map<string, Rule> getRules() {
        map<string, Rule> rules;
        for (auto& [originalSymbol, originalRule] : canonicalRules) {
            rules[originalSymbol] = originalRule;

            string suffix = getSuffix(originalSymbol);

            if (suffix == "") continue;

            string symbol = originalSymbol;
            Rule rule = originalRule;

            if (suffix == "C") {
                symbol = reflectH(symbol);
                rule = reflectHRule(rule);
                rules[symbol] = rule;
                continue;
            }

            if (suffix == "V") {
                symbol = rotate(symbol);
                rule = rotateRule(rule);
                rules[symbol] = rule;
                continue;
            }

            if (suffix == "P") {
                symbol = reflectH(symbol); // Q
                rule = reflectHRule(rule);
                rules[symbol] = rule;

                symbol = rotate(symbol); // R
                rule = rotateRule(rule);
                rules[symbol] = rule;

                symbol = reflectH(symbol); // S
                rule = reflectHRule(rule);
                rules[symbol] = rule;

                continue;
            }

            if (suffix == "N" || suffix == "E") {
                for (int i = 0; i < 3; ++i) {
                    symbol = rotate(symbol);
                    rule = rotateRule(rule);
                    rules[symbol] = rule;
                }
                continue;
            }

            if (suffix == "NE") {
                for (int i = 0; i < 3; ++i) {
                    symbol = rotate(symbol);
                    rule = rotateRule(rule);
                    rules[symbol] = rule;
                }

                symbol = rotate(symbol);
                rule = rotateRule(rule);
                symbol = reflectH(symbol);
                rule = reflectHRule(rule);
                rules[symbol] = rule;

                for (int i = 0; i < 3; ++i) { // again but reflected
                    symbol = rotate(symbol);
                    rule = rotateRule(rule);
                    rules[symbol] = rule;
                }
                continue;
            }

            cerr << "Unknown suffix: " << suffix << endl;
        }
        return rules;
    }

    // useful for telling if a number wall is generated by this morphism
    // assumes rules are valid and grid.size() is a power of morphismSize
    // good space efficiency, decent time efficiency
    template<typename T>
    bool compareZeros(vector<vector<T>>& grid) {
        string startSymbol = units[1].getSymbol();
        int W = grid.size();
        map<string, Rule> rulesString = getRules();

        // create symbolToId and idToSymbol
        // assumes each symbol is unique
        unordered_map<string, int> symbolToId;
        vector<string> idToSymbol;
        int c = 0;
        for (auto& [symbol, rule] : rulesString) {
            idToSymbol.push_back(symbol);
            symbolToId[symbol] = c;
            ++c;
        }

        // create rules map
        unordered_map<int, vector<vector<int>>> rules;
        int blockH = rulesString[startSymbol].size();
        int blockW = rulesString[startSymbol][0].size();
        for (auto& [symbol, rule] : rulesString) {
            vector<vector<int>> block;
            for (int r = 0; r < blockH; ++r) {
                block.push_back({});
                for (int c = 0; c < blockW; ++c) {
                    block[r].push_back(symbolToId[rule[r][c]]);
                }
            }
            rules[symbolToId[symbol]] = block;
        }

        int symbol = symbolToId[startSymbol];
        return compareZerosRecurse(grid, rules, 0, 0, W, symbol);
    }

    template<typename T>
    bool compareZerosRecurse(vector<vector<T>>& grid,
        unordered_map<int, vector<vector<int>>>& rules,
        int row, int col, int size, int symbol) {

        // base case
        if (size == 1) {
            bool isZero = (grid[row][col] == 0);
            if (symbol == 0) return isZero;
            else return !isZero;
        }

        // recursion
        auto& rule = rules[symbol];
        int blockSize = size / morphismSize;

        for (int r = 0; r < morphismSize; ++r) {
            for (int c = 0; c < morphismSize; ++c) {
                if (!compareZerosRecurse(
                        grid,
                        rules,
                        row + r * blockSize,
                        col + c * blockSize,
                        blockSize,
                        rule[r][c])) {
                    return false;
                }
            }
        }
        return true;
    }

    // alternate version for Flat walls
    template<typename T>
    bool compareZeros(vector<T>& grid) {
        string startSymbol = units[1].getSymbol();
        int W = sqrt(grid.size());
        map<string, Rule> rulesString = getRules();

        // create symbolToId and idToSymbol
        // assumes each symbol is unique
        unordered_map<string, int> symbolToId;
        vector<string> idToSymbol;
        int c = 0;
        for (auto& [symbol, rule] : rulesString) {
            idToSymbol.push_back(symbol);
            symbolToId[symbol] = c;
            ++c;
        }

        // create rules map
        unordered_map<int, vector<vector<int>>> rules;
        int blockH = rulesString[startSymbol].size();
        int blockW = rulesString[startSymbol][0].size();
        for (auto& [symbol, rule] : rulesString) {
            vector<vector<int>> block;
            for (int r = 0; r < blockH; ++r) {
                block.push_back({});
                for (int c = 0; c < blockW; ++c) {
                    block[r].push_back(symbolToId[rule[r][c]]);
                }
            }
            rules[symbolToId[symbol]] = block;
        }

        int symbol = symbolToId[startSymbol];
        return compareZerosRecurse(grid, rules, 0, 0, W, symbol, W);
    }

    template<typename T>
    bool compareZerosRecurse(vector<T>& grid,
        unordered_map<int, vector<vector<int>>>& rules,
        int row, int col, int size, int symbol, int gridWidth) {

        // base case
        if (size == 1) {
            bool isZero = (grid[row * gridWidth + col] == 0);
            if (symbol == 0) return isZero;
            else return !isZero;
        }

        // recursion
        auto& rule = rules[symbol];
        int blockSize = size / morphismSize;

        for (int r = 0; r < morphismSize; ++r) {
            for (int c = 0; c < morphismSize; ++c) {
                if (!compareZerosRecurse(
                        grid,
                        rules,
                        row + r * blockSize,
                        col + c * blockSize,
                        blockSize,
                        rule[r][c],
                        gridWidth)) {
                    return false;
                }
            }
        }
        return true;
    }
};



// ==============================================

/*

struct ZeroSquare {
    int bottom;
    int l;
    int rA, rB, rC;
    int A0, Al; // = B0, Cl, respectively
    // don't need D0=C0 or Dl=Bl because they appear in bottom row
    vector<int> Ek, Fk, Gk;
    // builds Hk as we go down the wall so that
    //   it is ready by the time we need it

    // ctor
    ZeroSquare(int topRow, int l) {
        Ek.reserve(l); // wait! won't know l at ctor time!
        /////////////////////////////
    }

    void updateHk(int Bk, int Ck) {

    }
};

// This is a hybrid of SquareNumberWall and Morphism that
//   finds the morphism as it generates the wall, allowing
//   it to only store two rows of the number wall (plus
//   some data on zero regions) instead of the whole wall
//   This takes the memory from O(n^2) to O(n)
struct SquareWallMorphism: public Morphism {
    // Morphism stuff (not already included in Morphism)
    int chunkWidth;

    // NumberWall stuff
    int modulo;
    int width;

    // SquareWallMorphism ctor
    SquareWallMorphism(vector<int>& S, int modulo,
            int morphismSize, int minUniqueIter):
            Morphism{morphismSize}, modulo{modulo},
            width{static_cast<int>(S.size())},
            chunkWidth{static_cast<int>(pow(morphismSize, minUniqueIter))} {
        
        Builder{*this, S, minUniqueIter};
    }

    // helper struct to keep track of data needed to build the SquareWallMorphism
    struct Builder {
        SquareWallMorphism& W;

        vector<int> prev;
        vector<int> prev2;
        unordered_map<int, ZeroSquare> zeros;
        // zeros[i] = info about zero square with
        //     leftmost col (outside window) = i
        vector<int> chunk;
        vector<int> nextChunk;

        int modulo;
        int width;
        int chunkWidth;
        // these are here simply so I can write modulo instead of W.modulo
        // This makes the code cleaner and is, in my opinion, worth three ints of memory

        vector<int> inverse;
        // a list of inverses of numbers mod modulo
        // (can be used in place of an inverse function)

        Builder(SquareWallMorphism& W, vector<int>& S, int minUniqueIter):
                W{W}, modulo{W.modulo}, width{W.width},
                chunkWidth{static_cast<int>(pow(W.morphismSize, minUniqueIter))} {

            // get inverses
            for (int i = 0; i < W.modulo; ++i) {
                inverse.push_back(modPow(i, W.modulo - 2, W.modulo));
            }

            // reserve prev vectors and chunks
            chunk.reserve(chunkWidth * chunkWidth);
            nextChunk.reserve(chunkWidth * chunkWidth);
            prev2.reserve(S.size());
            prev.reserve(S.size());

            // loop through first row
            /////////////////////////////


            // loop through wall while constructing it to find the morphism
            int numChunksWide = width / chunkWidth;
            for (int chunkRow = 0; chunkRow < numChunksWide; ++chunkRow) {
                createChunkRow(numChunksWide, chunkRow);
            }
        }

        void createChunkRow(int numChunksWide, int chunkRow) {
            createFirstChunk(chunkRow);
            for (int chunkCol = 1; chunkCol < numChunksWide - 1; ++chunkCol) {
                createChunk(chunkRow, chunkCol);

                // check if chunk is new (add it if so)
                // actually, only need to do this up to col,row / morphismSize
                //     because if we find a new symbol here, then we know
                //     the morphism finder won't succeed
                // also must update lastUnitPosition
                //     (only need to store one lastUnitPosition because the order
                //     the SuperChunks are found is the same order the regular Chunks
                //     are found, just scaled up)

                // check if (position // morphismSize) matches lastUnitPosition
                //     (then, do position % morphismSize to find which part of the unit it is)
                // if so, update the coresponding rule

                //////////////////////
            }
            createLastChunk(chunkRow);
        }

        // This is the most important method.
        //     It performs all the required number wall logic to get each new chunk.
        //     It is important that this be as efficient as possible.
        void createChunk(int chunkRow, int chunkCol) {
            
            chunk = move(nextChunk);

            // make first two rows of nextChunk (and 2nd row, last element of chunk)

            //   d
            // a b c
            //   x      (x = next one)
            //
            // Must check if a and/or c are zero, and if so,
            //     (and b not 0) update their squares

            // move down chunk/nextChunk in a paralellogram path creating each row
            for (int row = 2; row < chunkWidth; ++row) {
                
            }

            /////////////////////////////
            updatePrev(chunkCol);
        }

        void createFirstChunk(int chunkRow) { // has zeros in left triangle
            
            /////////////////////////////
            updatePrev(0);
        }

        void createLastChunk(int chunkRow) { // has zeros in right triangle
            
            ///////////////////////////////
            // don't update prev
        }

        // copies last two rows of chunk into prev and prev2
        void updatePrev(int chunkCol) {
            auto chunkLastRow = chunk.begin() + chunkWidth * (chunkWidth - 1);
            auto prev2Start = prev2.begin() + chunkCol * chunkWidth;
            auto prevStart = prev.begin() + chunkCol * chunkWidth;

            copy_n(chunkLastRow, chunkWidth, prev2Start);
            copy_n(chunkLastRow - chunkWidth, chunkWidth, prevStart);
        }
    }; // Builder
};

*/




/* // Row of Chunks by row of Chunks
for (int row = 0; row < maxrow; ++row) {
    prev2 = prev;
    prev = next;
    // first/last columns are beside zero squares,
    //   so they are geometric sequences
    next[0] = prev[0] * prev[0] * inverse[prev2[0]] % modulo;

    // make middle columns
    for (int col = 0; col < )
    next[width - 1] = prev[width - 1] * prev[width - 1]
                      * inverse[prev2[width - 1]] % modulo;
}
// */