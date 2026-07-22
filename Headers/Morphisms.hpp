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
        throw runtime_error("stbi_load failed: " + filename);
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
    for (auto it = symbol.rbegin(); it != symbol.rend() && std::isupper(*it); ++it) {
        char& c = *it;
    
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
    return symbol;
}

// flips left and right
string reflectH(string symbol) {
    for (auto it = symbol.rbegin(); it != symbol.rend() && std::isupper(*it); ++it) {
        char& c = *it;
    
        if (c == 'E') c = 'W';
        else if (c == 'W') c = 'E';
        
        else if (c == 'C') c = 'A';
        else if (c == 'A') c = 'C';

        else if (c == 'P') c = 'Q';
        else if (c == 'Q') c = 'P';
        else if (c == 'R') c = 'T';
        else if (c == 'T') c = 'R';
    }
    return symbol;
}

// specific version for uint8_t
void printChunk(vector<uint8_t>& chunk, int chunkWidth = 0) {
    if (chunkWidth == 0) {
        chunkWidth = sqrt(chunk.size());
    }
    for (int row = 0; row < chunkWidth; ++row) {
        for (int col = 0; col < chunkWidth; ++col) {
            cout << +chunk[row * chunkWidth + col] << " ";
        }
        cout << endl;
    }
}

template <typename T>
void printChunk(vector<T>& chunk, int chunkWidth = 0) {
    if (chunkWidth == 0) {
        chunkWidth = sqrt(chunk.size());
    }
    for (int row = 0; row < chunkWidth; ++row) {
        for (int col = 0; col < chunkWidth; ++col) {
            cout << chunk[row * chunkWidth + col] << " ";
        }
        cout << endl;
    }
}



// In the case of (5,2)-Cantor, units are 25 x 25 grids
//   that can uniquely determine what symbol you have
struct Unit {
    string type;
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

    Unit(string type, int index, string canVar, Chunk chunk, int row, int col):
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
        if (type == "0") return "0";
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

        if (sideLength == 0) {
            throw invalid_argument("Sequence too small, or morphism size, min_iters_for_unique too large");
        }

        // add 0 unit
        Chunk zeroChunk(grid[0][0].size(), 0);
        units.emplace_back("0", 0, "", zeroChunk, 0, 0);

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

    // first output is whether or not a new unit was created
    // second output is a pointer to the unit (new or old) in the Morphism
    pair<bool, Unit*> addUnit(Chunk& chunk, int row, int col) {
        Unit* unit = findInMorphism(chunk);
        if (unit) return {false, unit};

        string type = getType(chunk);

        int typeIdx = indexToSymbol(type);
        int unitIdx = typeCounts[typeIdx];
        string canVar = getCanonicalVariant(chunk);

        units.emplace_back(type, unitIdx, canVar, chunk, row, col);

        return {true, &units.back()};
    }

    Unit* findInMorphism(Chunk& chunk) {
        for (auto& u : units) {
            if (u.isEquivalent(chunk)) return &u;
        }
        return nullptr;
    }

    bool inMorphism(Chunk& chunk) {
        return (findInMorphism(chunk));
    }

    string indexToSymbol(int idx) {
        int len = 1;
        int count = 26;

        // find length of the output.
        while (idx >= count) {
            idx -= count;
            ++len;
            count *= 26;
        }

        string s(len, 'A');

        // write the base-26 digits.
        for (int i = len - 1; i >= 0; i--) {
            s[i] = 'A' + (idx % 26);
            idx /= 26;
        }

        return s;
    }

    int indexToSymbol(const string& s) {
        int offset = 0;
        int count = 26;

        for (int len = 1; len < s.size(); ++len) {
            offset += count;
            count *= 26;
        }

        int value = 0;
        for (char c : s) {
            value = 26 * value + (c - 'A');
        }

        return offset + value;
    }

    // assumes chunk is not all zeros
    string getType(Chunk& chunk) {
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
            return indexToSymbol(index);
        } else { // create new type
            string newType = indexToSymbol(types.size());
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
        throw string("Rule Not Found");
    }

    string getSuffix(const string& symbol) const {
        if (symbol == "0") return "";

        size_t i = symbol.size();
        while (i > 0 && isupper(static_cast<unsigned char>(symbol[i - 1]))) {
            --i;
        }
        return symbol.substr(i);
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

            throw string("Unknown suffix: " + suffix);
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
            else throw string("Unknown suffix: " + suffix);
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

            throw string("Unknown suffix: " + suffix);
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



//*



// This is a hybrid of SquareNumberWall and Morphism that
//   finds the morphism as it generates the wall, allowing
//   it to only store two rows of the number wall (plus
//   some data on zero regions) instead of the whole wall
//   This brings the memory from O(n^2) to O(n)
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
        
        if (S[0] % modulo == 0 || S.back() % modulo == 0) {
            throw invalid_argument("Sequence may not start or end with 0");
        }

        // add 0 unit
        int chunkCapacity = pow(morphismSize, 2 * minUniqueIter);
        Chunk zeroChunk(chunkCapacity, 0);
        units.emplace_back("0", 0, "", zeroChunk, 0, 0);
        canonicalRules["0"] = Rule(morphismSize, vector<string>(morphismSize, "0"));

        Builder{*this, S, minUniqueIter};
    }

    // helper struct to keep track of data needed to build the SquareWallMorphism
    struct Builder {
        SquareWallMorphism& W;

        vector<int> prev;
        vector<int> prev2;

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

        int newChunkCutoff;
        unordered_map<pair<int, int>, int, PairHash> toProcess;
        // toProcess[{r,c}] = index of unit in units at row r, col c

        struct ZeroSquare {
            Builder* B;
            int top;
            int bottom;
            int l;
            int rA, rB, rC, rD;
            int A0, AlPlus1; // = B0, ClPlus1 respectively
            // don't need D0 = C0 or DlPlus1 = BlPlus1 because
            //     they appear in bottom row
            vector<int> Ek, Fk, GkRev;
            // builds Ek, Fk, and Gk as we go down the wall so that
            //     they are ready by the time we need them
            // GkRev is Gk flipped front to back (GkRev[i] = Gk[(l+1)-i])
            //     This is so it can be built from the top down

            // ctor
            // Note this does not set the first element of Ak
            //     That must be done manually after
            ZeroSquare(Builder* B, int top, int rA, int rB, int A0):
                    B{B}, top{top}, bottom{top + 1}, rA{rA}, rB{rB}, A0{A0} {}

            void setEnd(int x, int s, int z) {
                AlPlus1 = s;
                rC = s * B->inverse[x] % B->modulo;
                l = -z;
                rD = rB * rC * B->inverse[rA] % B->modulo;
                if (l % 2 == 1) {
                    rD = B->modulo - rD;
                }
            }

            int getHk(int w, int s) {
                int k = l + w + 1;
                int Ak = A0 * modPowPrime(rA, k, B->modulo) % B->modulo;
                int x = rB * Ek[k - 1] * B->inverse[Ak] % B->modulo;

                int Bk = A0 * modPowPrime(rB, k, B->modulo) % B->modulo; // B0 = A0
                if (k % 2 == 0) {
                    x += rA * Fk[k - 1] * B->inverse[Bk] % B->modulo;
                } else {
                    x += B->modulo - (rA * Fk[k - 1] * B->inverse[Bk] % B->modulo);
                }

                int Ck = AlPlus1 * modPowPrime(B->inverse[rC], l - k + 1, B->modulo) % B->modulo; // Cl = Al
                if (k % 2 == 0) {
                    x += B->modulo - (rD * GkRev[l - k] * B->inverse[Ck] % B->modulo);
                } else {
                    x += rD * GkRev[l - k] * B->inverse[Ck] % B->modulo;
                }

                x *= s * B->inverse[rC];
                return x % B->modulo;
            }
        };
        unordered_map<int, vector<ZeroSquare>> zeros;
        // zeros[i] = info about all zero squares with
        //     leftmost col (outside window) = i
        //     (distinguish between zero squares at
        //     left point i using the height range)

        // ctor
        Builder(SquareWallMorphism& W, vector<int>& S, int minUniqueIter):
                W{W}, modulo{W.modulo}, width{W.width},
                chunkWidth{static_cast<int>(pow(W.morphismSize, minUniqueIter))} {

            // get inverses
            for (int i = 0; i < W.modulo; ++i) {
                inverse.push_back(modPow(i, W.modulo - 2, W.modulo));
            }

            // set the number of chunks to be a multiple of the morphism size
            int numChunksWide = width / chunkWidth / W.morphismSize * W.morphismSize;
            int newWidth = numChunksWide * chunkWidth;
            newChunkCutoff = numChunksWide / W.morphismSize;

            // resize prev vectors and chunks
            chunk.resize(chunkWidth * chunkWidth);
            nextChunk.resize(chunkWidth * chunkWidth);
            prev2.resize(newWidth + 1); // a zero is added to the end of prev2 and prev so that the
            prev.resize(newWidth + 1);  //     last createChunk() doesn't access out of bounds
            prev2[newWidth] = 0;
            prev[newWidth] = 0;

            // loop through first row
            createFirstChunkRow(numChunksWide, S, newWidth);

            // loop through wall while constructing it to find the morphism
            for (int chunkRow = 1; chunkRow < numChunksWide; ++chunkRow) {
                createChunkRow(numChunksWide, chunkRow);
            }
        }

        void createChunkRow(int numChunksWide, int chunkRow) {

            // create the top-left triangle of the first chunk in the row
            //     each following chunk will use that of the one before it
            createFirstChunk(chunkRow);

            // create middle chunks
            for (int chunkCol = 0; chunkCol < numChunksWide - 1; ++chunkCol) {

                createChunk(chunkRow, chunkCol);
                processChunk(chunkRow, chunkCol);
            }

            // create the bottom-right triangle of the last chunk in the row
            //     (and process it as above)
            createLastChunk(chunkRow, numChunksWide - 1);
            processChunk(chunkRow, numChunksWide - 1);
        }

        // iterates through a paralelogram creating the missing entries
        //     of chunk and the entries of nextChunk required for the
        //     next call of this function
        void createChunk(int chunkRow, int chunkCol) {
            
            // put nextChunk in chunk (and preserve nextChunk's length)
            swap(chunk, nextChunk);

            int    w;    // This is the relative location of each number wall entry
            int a, s, d; //     x is the one being created
            int z, x;

            // make first two rows of nextChunk from prev and prev2
            // (keep in mind, staircase => 2nd row will have 1 element in chunk)
            int idx = 0;
            int ROW = chunkRow * chunkWidth;
            int COL = (chunkCol + 1) * chunkWidth;

            // first row
            s = prev[COL - 1]; // = first a
            d = prev[COL]; // = first s
            x = chunk[chunkWidth - 1]; // = first z

            while (idx < chunkWidth) {
                w = prev2[COL];
                a = s;
                s = d;
                d = prev[COL + 1];
                z = x;

                x = getElement(w, a, s, d, z, ROW, COL);
                nextChunk[idx] = x;

                ++idx;
                ++COL;
            }

            ++ROW;
            COL = (chunkCol + 1) * chunkWidth - 1;

            // first element of second row
            w = prev[COL];
            a = chunk[chunkWidth - 2];
            s = chunk[chunkWidth - 1];
            d = nextChunk[0];
            z = chunk[2 * chunkWidth - 2];

            x = getElement(w, a, s, d, z, ROW, COL);
            chunk[2 * chunkWidth - 1] = x;

            ++COL;

            // rest of 2nd row
            idx = chunkWidth;
            while (idx < 2 * chunkWidth - 1) {
                w = prev[COL];
                a = s;
                s = d;
                d = nextChunk[idx - chunkWidth + 1];
                z = x;

                x = getElement(w, a, s, d, z, ROW, COL);
                nextChunk[idx] = x;

                ++idx;
                ++COL;
            }

            // move down chunk/nextChunk in a paralellogram path creating each row >= 2
            //     It is important this be fast as this code causes the majority of the run time
            for (int row = 2; row < chunkWidth; ++row) {

                ROW = chunkRow * chunkWidth + row;
                COL = (chunkCol + 1) * chunkWidth - row;

                // all but last column in chunk (only needs info from chunk)
                idx = (row + 1) * chunkWidth - row;
                s = chunk[idx - chunkWidth - 1]; // = first a
                d = chunk[idx - chunkWidth]; // = first s
                x = chunk[idx - 1];// = first z
                while (idx < (row + 1) * chunkWidth - 1) {
                    w = chunk[idx - 2 * chunkWidth];
                    a = s;
                    s = d;
                    d = chunk[idx - chunkWidth + 1];
                    z = x;

                    x = getElement(w, a, s, d, z, ROW, COL);
                    chunk[idx] = x;

                    ++idx;
                    ++COL;
                }

                // last column of chunk (needs d from nextChunk)
                w = chunk[idx - 2 * chunkWidth];
                a = s;
                s = d;
                d = nextChunk[(row - 1) * chunkWidth];
                z = x;
                
                x = getElement(w, a, s, d, z, ROW, COL);
                chunk[idx] = x;
                
                ++COL;

                // all columns in nextChunk (only needs info from nextChunk)
                //     (a and z are still in chunk, but they get their info from s and x)
                idx = row * chunkWidth;
                while (idx < (row + 1) * chunkWidth - row) {
                    w = nextChunk[idx - 2 * chunkWidth];
                    a = s;
                    s = d;
                    d = nextChunk[idx - chunkWidth + 1];
                    z = x;

                    x = getElement(w, a, s, d, z, ROW, COL);
                    nextChunk[idx] = x;

                    idx++;
                    ++COL;
                }
            }

            updatePrev(chunkCol);
        }

        // This is the most important method. It performs
        //     all the required number wall logic to get each new element of
        //     the wall. It is important that this be as efficient as possible.
        // Note: zeros are stored as negative numbers representing how
        //     far from the left edge of the squere the given zero is
        //     (The exception of this is the infinite zero squares to the left,
        //     right, and top which are literal 0s)
        //     (the zero squares of literal 0s will not be updated)
        int getElement(int w, int a, int s, int d, int z, int ROW, int COL) {

            // get x
            int x;
            if (w > 0) {
                if (a > 0 && d > 0) {
                    if (s > 0) {
                        x = ((s * s + (modulo - a) * d) * inverse[w]) % modulo;
                    } else { // s == 0
                        x = ((modulo - a) * d * inverse[w]) % modulo;
                    }
                } else { // a or d == 0
                    if (s > 0) {
                        x = (s * s) * inverse[w] % modulo;
                    } else { // s == 0
                        x = 0;
                    }
                }
            }

            else if (s < 0) { // x = Dk or 0
                if (z < 0) { // quick easy check
                    x = 0;
                }
                else {
                    // must find zero square containing s
                    auto [zs, zsit] = findZeroSquare(COL + s, ROW - 1);
                    if (ROW < zs->bottom) { // x in zs
                        x = 0;
                    } else {
                        x = z * inverse[zs->rD] % modulo; // = z * rD^-1
                    }
                }
            }

            else { // x = Hk
                // must find zero square containing w
                auto [zs, zsit] = findZeroSquare(COL + w, ROW - 2);
                x = zs->getHk(w, s);

                if (zs->l + w == 0) { // if last Hk, destroy zs
                    zeros[COL + w].erase(zsit);
                }
            }


            // if zero, assign correct negative value
            if (x == 0) {
                if (z < 0) {
                    x = z - 1;
                    if (s > 0) { // update Ek //////////////////////////// ???

                    }
                }
                else {
                    x = -1;
                    if (s > 0) { // x = first zero in a zero square
                        zeros[COL - 1].emplace_back(
                                this, // B
                                ROW - 1, // top
                                s * inverse[a] % modulo, // rA
                                z * inverse[a], // rB
                                a); // A0
                    }
                }
            }
            else if (z < 0 && a > 0) { // z = last zero in square
                auto [zs, zsit] = findZeroSquare(COL - 1 + z, ROW);
                // l is set here, bottom is incremented for each Ek
                //     (At this stage, bottom - top should = l)
                zs->setEnd(x, s, z);
            }

            // if s != 0 and x, a, or d == 0, then update coresponding zero square
            //     (this will run even if the zero square was just created)
            //     (Note this will NOT run if the values are a literal 0,  i.e., if the
            //     zero square is one of the infitite ones to the top, left, or right)
            if (s > 0) {
                if (x < 0) { // update Ek of square at x
                    auto [zs, zsit] = findZeroSquare(COL + x, ROW);
                    if (w > 0) {
                        zs->Ek.push_back(w);
                    } else {
                        zs->Ek.push_back(0);
                    }
                    // Increment bottom for each Ek
                    // when the end of zs has been found, bottom - top should = l
                    zs->bottom++;
                }
                if (a < 0) { // update GkRev of square at a
                    auto [zs, zsit] = findZeroSquare(COL - 1 + a, ROW - 1);
                    if (d > 0) {
                        zs->GkRev.push_back(d);
                    } else {
                        zs->GkRev.push_back(0);
                    }
                }
                if (d < 0) { // update Fk of square at d
                    auto [zs, zsit] = findZeroSquare(COL + 1 + d, ROW - 1);
                    if (a > 0) {
                        zs->Fk.push_back(a);
                    } else {
                        zs->Fk.push_back(0);
                    }
                }
            }

            return x;
        }

        // finds the zero square with left edge x and with
        //     y somewhere in its height range
        // (y can be the row directly beneath the square, but not the one above)
        pair<ZeroSquare*, vector<ZeroSquare>::iterator> findZeroSquare(int x, int y) {
            auto mapIt = zeros.find(x);
            if (mapIt == zeros.end()) return {nullptr, {}};

            auto& vec = mapIt->second;
            for (auto it = vec.begin(); it != vec.end(); ++it) {
                if (it->bottom >= y && y > it->top) // top < y < bottom because down = larger row
                    return {&*it, it};
            }
            return {nullptr, vec.end()};
        }

        // copies last two rows of chunk into prev and prev2
        void updatePrev(int chunkCol) {
            auto chunkLastRow = chunk.begin() + chunkWidth * (chunkWidth - 1);
            auto prev2Start = prev2.begin() + chunkCol * chunkWidth;
            auto prevStart = prev.begin() + chunkCol * chunkWidth;

            copy_n(chunkLastRow, chunkWidth, prevStart);
            copy_n(chunkLastRow - chunkWidth, chunkWidth, prev2Start);
        }

        // This is where most of the morphism finding logic occurs
        // Unlike the original Morphism, this runs through each chunk only once
        void processChunk(int chunkRow, int chunkCol) { ///////////////////////

            // convert to Morphism-style chunk
            Chunk chunk0;
            chunk0.reserve(chunk.size());
            for (int x : chunk) {
                chunk0.push_back(x < 0 ? 0 : 1);
                // change 1 to static_cast<uint8_t>(x) if you want to
                //     differentiate between non-zeros
            }

            // if new, add to Morphism
            auto [isNew, unit] = W.addUnit(chunk0, chunkRow, chunkCol);

            if (isNew && (chunkRow >= newChunkCutoff || chunkCol >= newChunkCutoff)) {
                throw string("New Unit found past threshold");
            }

            // update toProcess and canonicalRules
            if (isNew) {
                toProcess[{chunkRow, chunkCol}] = W.units.size() - 1;
                W.canonicalRules[unit->getSymbol()]
                    = vector(W.morphismSize, vector<string>(W.morphismSize));
            }

            auto it = toProcess.find({chunkRow / W.morphismSize, chunkCol / W.morphismSize});
            if (it != toProcess.end()) { // need to update a previous unit
                
                string symbol = W.getSymbol(chunk0);
                // can be slightly optimized later, since we already know what unit
                Unit* unitToUpdate = &(W.units[it->second]);
                string symbolToUpdate = unitToUpdate->getSymbol();

                int r = chunkRow % W.morphismSize;
                int c = chunkCol % W.morphismSize;

                W.canonicalRules[symbolToUpdate][r][c] = symbol;

                // remove unitToUpdate from toProcess if we're done with it
                if (r == W.morphismSize - 1 && c == W.morphismSize - 1) {
                    toProcess.erase(it);
                }
            }
            

            /////////////////////

            // temp code {
            //printChunk(chunk, chunkWidth);
            //cout << endl;
            // }

            //////////////////////
        }

        // The following methods are similar to those above, but for edge cases
        //     (Mostly literal edge cases, like the first row of the wall
        //     or the first/last column of a row)

        // creates top-left triangle in first chunk
        //     (stores it in nextChunk because createChunk() will move it over)
        //     (zeros to the left)
        void createFirstChunk(int chunkRow) {

            int    w;    // This is the relative location of each number wall entry
            int a, s, d; //     x is the one being created
            int z, x;

            // make first two rows of nextChunk from prev and prev2
            int idx = 0;
            int ROW = chunkRow * chunkWidth;
            int COL = 0;

            // first row

            // first element of first row
            w = prev2[COL]; // guaranteed to be > 0 because zero square to the left => FC1
            a = 0;
            s = prev[COL]; // same with s
            d = prev[COL + 1];
            z = 0;

            x = s * s * inverse[w] % modulo; // a * d = 0 & s > 0
            nextChunk[idx] = x;

            // only d could be a zero square in zeros
            //     if so, update its Fk
            if (d < 0) {
                auto [zs, zsit] = findZeroSquare(COL + 1 + d, ROW - 1);
                zs->Fk.push_back(0); // a = 0
            }

            // x != 0, so don't need to check for new 0 square

            ++idx;
            ++COL;

            // rest of first row
            while (idx < chunkWidth) {
                w = prev2[COL];
                a = s;
                s = d;
                d = prev[COL + 1];
                z = x;

                x = getElement(w, a, s, d, z, ROW, COL);
                nextChunk[idx] = x;

                ++idx;
                ++COL;
            }

            ++ROW;
            idx = chunkWidth;
            COL = 0;

            // first element of second row
            w = prev[COL]; // guaranteed to be > 0 because zero square to the left => FC1
            a = 0;
            s = nextChunk[COL]; // same with s
            d = nextChunk[COL + 1];
            z = 0;

            x = s * s * inverse[w] % modulo; // a * d = 0 & s > 0
            nextChunk[idx] = x;

            // only d could be a zero square in zeros
            //     if so, update its Fk
            if (d < 0) {
                auto [zs, zsit] = findZeroSquare(COL + 1 + d, ROW - 1);
                zs->Fk.push_back(0); // a = 0
            }

            // x != 0, so don't need to check for new 0 square

            ++idx;
            ++COL;

            // rest of second row
            while (idx < 2 * chunkWidth - 1) {
                w = prev[COL];
                a = s;
                s = d;
                d = nextChunk[COL + 1];
                z = x;

                x = getElement(w, a, s, d, z, ROW, COL);
                nextChunk[idx] = x;

                ++idx;
                ++COL;
            }

            // move down nextChunk in a top-left triangle creating each row >= 2
            for (int row = 2; row < chunkWidth; ++row) {

                ROW = chunkRow * chunkWidth + row;
                COL = 0;
                idx = row * chunkWidth;

                // first element of row
                w = nextChunk[idx - 2 * chunkWidth]; // guaranteed to be > 0 because zero square to the left => FC1
                a = 0;
                s = nextChunk[idx - chunkWidth]; // same with s
                d = nextChunk[idx - chunkWidth + 1];
                z = 0;

                x = s * s * inverse[w] % modulo; // a * d = 0 & s > 0
                nextChunk[idx] = x;

                // only d could be a zero square in zeros
                //     if so, update its Fk
                if (d < 0) {
                    auto [zs, zsit] = findZeroSquare(COL + 1 + d, ROW - 1);
                    zs->Fk.push_back(0); // a = 0
                }

                // x != 0, so don't need to check for new 0 square

                ++idx;
                ++COL;

                // rest of row
                while (idx < (row + 1) * chunkWidth - row) {
                    w = nextChunk[idx - 2 * chunkWidth];
                    a = s;
                    s = d;
                    d = nextChunk[idx - chunkWidth + 1];
                    z = x;

                    x = getElement(w, a, s, d, z, ROW, COL);
                    nextChunk[idx] = x;

                    idx++;
                    ++COL;
                }
            }

            // don't updatePrev()
        }

        // creates bottom-right triangle of last chunk
        //     (zeros to the right)
        void createLastChunk(int chunkRow, int lastChunkIndex) {

            // put nextChunk in chunk (and preserve nextChunk's length)
            swap(chunk, nextChunk);
            
            int    w;    // This is the relative location of each number wall entry
            int a, s, d; //     x is the one being created
            int z, x;

            // make first two rows of nextChunk from prev and prev2
            // (Note this is actually only one element since it
            //     is the bottom-right triangle we are creating)
            int idx = 2 * chunkWidth - 1;
            int ROW = chunkRow * chunkWidth + 1;
            int COL = (lastChunkIndex + 1) * chunkWidth - 1;

            // last (and only) element of second row
            w = prev[COL]; // guaranteed to be > 0 because zero square to the right => FC1
            a = chunk[idx - chunkWidth - 1];
            s = chunk[idx - chunkWidth]; // same with s
            d = 0;
            z = chunk[idx - 1];

            x = s * s * inverse[w] % modulo; // a * d = 0 & s > 0
            chunk[idx] = x;

            // only a could be a zero square in zeros
            //     if so, update its GkRev
            if (a < 0) {
                auto [zs, zsit] = findZeroSquare(COL - 1 + a, ROW - 1);
                zs->GkRev.push_back(0); // d = 0
            }
            else if (z < 0) {
                // z could also be the end of a zero square
                //     (if a > 0 and z < 0)
                auto [zs, zsit] = findZeroSquare(COL - 1 + z, ROW);
                // l is set here, bottom is incremented for each Ek
                //     (At this stage, bottom - top should = l)
                zs->setEnd(x, s, z);
            }

            // move down nextChunk in a bottom-right triangle creating each row >= 2
            for (int row = 2; row < chunkWidth; ++row) {

                ROW = chunkRow * chunkWidth + row;
                COL = (lastChunkIndex + 1) * chunkWidth - row;
                idx = (row + 1) * chunkWidth - row;

                // all but last element of row
                s = chunk[idx - chunkWidth - 1]; // first a
                d = chunk[idx - chunkWidth]; // first s
                x = chunk[idx - 1]; // first z

                while (idx < (row + 1) * chunkWidth - 1) {
                    w = chunk[idx - 2 * chunkWidth];
                    a = s;
                    s = d;
                    d = chunk[idx - chunkWidth + 1];
                    z = x;

                    x = getElement(w, a, s, d, z, ROW, COL);
                    chunk[idx] = x;

                    idx++;
                    ++COL;
                }

                // last element of row
                w = chunk[idx - 2 * chunkWidth]; // guaranteed to be > 0 because zero square to the right => FC1
                a = s;
                s = d; // same with s
                d = 0;
                z = x;

                x = s * s * inverse[w] % modulo; // a * d = 0 & s > 0
                chunk[idx] = x;

                // only a could be a zero square in zeros
                //     if so, update its GkRev
                if (a < 0) {
                    auto [zs, zsit] = findZeroSquare(COL - 1 + a, ROW - 1);
                    zs->GkRev.push_back(0); // d = 0
                }
                else if (z < 0) {
                    // z could also be the end of a zero square
                    //     (if a > 0 and z < 0)
                    auto [zs, zsit] = findZeroSquare(COL - 1 + z, ROW);
                    // l is set here, bottom is incremented for each Ek
                    //     (At this stage, bottom - top should = l)
                    zs->setEnd(x, s, z);
                }
            }

            updatePrev(lastChunkIndex);
        }

        // creates the first row of chunks in the number wall
        void createFirstChunkRow(int numChunksWide, vector<int>& S, int newWidth) {

            // create the top-left triangle of the first chunk in the row
            //     each following chunk will use that of the one before it
            createFirstChunkFirstRow(S);

            // create middle chunks
            for (int chunkCol = 0; chunkCol < numChunksWide - 1; ++chunkCol) {

                createChunkFirstRow(S, chunkCol);
                processChunk(0, chunkCol);
            }

            // create the bottom-right triangle of the last chunk in the row
            //     (and process it as above)
            // Note, the only place this differs from a regular createLastChunk()
            //     call is in the last element of the second row, where w is 1 instead
            //     of whatever's in prev. This can be fixed by setting this element of prev to 1
            prev[newWidth - 1] = 1;
            createLastChunk(0, numChunksWide - 1);
            processChunk(0, numChunksWide - 1);
        }

        // creates top-left triangle in first chunk
        //     (stores it in nextChunk because createChunk() will move it over)
        //     (zeros to the left)
        void createFirstChunkFirstRow(vector<int>& S) {

            int    w;    // This is the relative location of each number wall entry
            int a, s, d; //     x is the one being created
            int z, x;

            // make first two rows of nextChunk from prev and prev2
            int idx = 0;
            int ROW = 0;
            int COL = 0;

            // first row (just S)

            x = 0; // = first z
            while (idx < chunkWidth) {

                z = x;
                x = (S[idx] % modulo + modulo) % modulo; // first x != 0 because S can't start/end with 0

                if (x == 0) { // if 0, set to correct negative value
                    if (z < 0) {
                        x = z - 1;
                    }
                    else { // x > 0
                        x = -1;

                        // create new 0 square at x (and update Ek)
                        zeros[COL - 1].emplace_back(
                                this, // B
                                ROW - 1, // top
                                1, // rA (s = a = 1)
                                z, // rB (inverse[a] = 1)
                                1); // A0 (a = 1)
                    }
                    // regardless of new or not, update 0 square at x (we know s == 1)
                    auto [zs, zsit] = findZeroSquare(COL + x, ROW);
                    zs->Ek.push_back(0); // w = 0
                    zs->bottom++;
                }
                else { // x > 0
                    if (z < 0) {
                        // end of z's 0 square
                        auto [zs, zsit] = findZeroSquare(COL - 1 + z, ROW);
                        zs->setEnd(x, 1, z); // (s = 1)
                    }
                }
                
                nextChunk[idx] = x;

                ++idx;
                ++COL;
            }

            ++ROW;
            COL = 0;
            idx = chunkWidth;

            // first element of second row
            w = 1; // whole prev = 1
            a = 0;
            s = nextChunk[COL]; // > 0
            d = nextChunk[COL + 1];
            z = 0;

            x = s * s % modulo; // a * d = 0 & s > 0 & inverse[w] = 1
            nextChunk[idx] = x;

            // only d could be a zero square in zeros
            //     if so, update its Fk
            if (d < 0) {
                auto [zs, zsit] = findZeroSquare(COL + 1 + d, ROW - 1);
                zs->Fk.push_back(0); // a = 0
            }

            ++idx;
            ++COL;

            // rest of second row
            while (idx < 2 * chunkWidth - 1) {
                // w = 1
                a = s;
                s = d;
                d = nextChunk[COL + 1];
                z = x;

                x = getElement(w, a, s, d, z, ROW, COL);
                nextChunk[idx] = x;

                ++idx;
                ++COL;
            }

            // move down nextChunk in a top-left triangle creating each row >= 2
            for (int row = 2; row < chunkWidth; ++row) {

                ROW = row;
                COL = 0;
                idx = row * chunkWidth;

                // first element of row
                w = nextChunk[idx - 2 * chunkWidth]; // guaranteed to be > 0 because zero square to the left => FC1
                a = 0;
                s = nextChunk[idx - chunkWidth]; // same with s
                d = nextChunk[idx - chunkWidth + 1];
                z = 0;

                x = s * s * inverse[w] % modulo; // a * d = 0 & s > 0
                nextChunk[idx] = x;

                // only d could be a zero square in zeros
                //     if so, update its Fk
                if (d < 0) {
                    auto [zs, zsit] = findZeroSquare(COL + 1 + d, ROW - 1);
                    zs->Fk.push_back(0); // a = 0
                }

                ++idx;
                ++COL;

                // rest of row
                while (idx < (row + 1) * chunkWidth - row) {
                    w = nextChunk[idx - 2 * chunkWidth];
                    a = s;
                    s = d;
                    d = nextChunk[idx - chunkWidth + 1];
                    z = x;

                    x = getElement(w, a, s, d, z, ROW, COL);
                    nextChunk[idx] = x;

                    idx++;
                    ++COL;
                }
            }

            // don't updatePrev()
        }

        void createChunkFirstRow(vector<int>& S, int chunkCol) {
            
            // put nextChunk in chunk (and preserve nextChunk's length)
            swap(chunk, nextChunk);

            int    w;    // This is the relative location of each number wall entry
            int a, s, d; //     x is the one being created
            int z, x;

            // make first two rows of nextChunk from prev and prev2
            // (keep in mind, staircase => 2nd row will have 1 element in chunk)
            int idx = 0;
            int ROW = 0;
            int COL = (chunkCol + 1) * chunkWidth;

            // first row (just S)

            x = chunk[chunkWidth - 1]; // = first z
            while (idx < chunkWidth) {

                z = x;
                x = (S[COL] % modulo + modulo) % modulo; // first x != 0 because S can't start/end with 0

                if (x == 0) { // if 0, set to correct negative value
                    if (z < 0) {
                        x = z - 1;
                    }
                    else { // x > 0
                        x = -1;

                        // create new 0 square at x (and update Ek)
                        zeros[COL - 1].emplace_back(
                                this, // B
                                ROW - 1, // top
                                1, // rA (s = a = 1)
                                z, // rB (inverse[a] = 1)
                                1); // A0 (a = 1)
                    }
                    // regardless of new or not, update 0 square at x (we know s == 1)
                    auto [zs, zsit] = findZeroSquare(COL + x, ROW);
                    zs->Ek.push_back(0); // w = 0
                    zs->bottom++;
                }
                else { // x > 0
                    if (z < 0) {
                        // end of z's 0 square
                        auto [zs, zsit] = findZeroSquare(COL - 1 + z, ROW);
                        zs->setEnd(x, 1, z); // (s = 1)
                    }
                }
                
                nextChunk[idx] = x;

                ++idx;
                ++COL;
            }

            ++ROW;
            COL = (chunkCol + 1) * chunkWidth - 1;

            // first element of second row
            w = 1;
            a = chunk[chunkWidth - 2];
            s = chunk[chunkWidth - 1];
            d = nextChunk[0];
            z = chunk[2 * chunkWidth - 2];

            x = getElement(w, a, s, d, z, ROW, COL);
            chunk[2 * chunkWidth - 1] = x;

            ++COL;

            // rest of 2nd row
            idx = chunkWidth;
            while (idx < 2 * chunkWidth - 1) {
                // w = 1
                a = s;
                s = d;
                d = nextChunk[idx - chunkWidth + 1];
                z = x;

                x = getElement(w, a, s, d, z, ROW, COL);
                nextChunk[idx] = x;

                ++idx;
                ++COL;
            }

            // move down chunk/nextChunk in a paralellogram path creating each row >= 2
            //     It is important this be fast as this code causes the majority of the run time
            for (int row = 2; row < chunkWidth; ++row) {

                ROW = row;
                COL = (chunkCol + 1) * chunkWidth - row;

                // all but last column in chunk (only needs info from chunk)
                idx = (row + 1) * chunkWidth - row;
                s = chunk[idx - chunkWidth - 1]; // = first a
                d = chunk[idx - chunkWidth]; // = first s
                x = chunk[idx - 1]; // = first z
                while (idx < (row + 1) * chunkWidth - 1) {
                    w = chunk[idx - 2 * chunkWidth];
                    a = s;
                    s = d;
                    d = chunk[idx - chunkWidth + 1];
                    z = x;

                    x = getElement(w, a, s, d, z, ROW, COL);
                    chunk[idx] = x;

                    ++idx;
                    ++COL;
                }

                // last column of chunk (needs d from nextChunk)
                w = chunk[idx - 2 * chunkWidth];
                a = s;
                s = d;
                d = nextChunk[(row - 1) * chunkWidth];
                z = x;
                
                x = getElement(w, a, s, d, z, ROW, COL);
                chunk[idx] = x;
                
                ++COL;

                // all columns in nextChunk (only needs info from nextChunk)
                //     (a and z are still in chunk, but they get their info from s and x)
                idx = row * chunkWidth;
                while (idx < (row + 1) * chunkWidth - row) {
                    w = nextChunk[idx - 2 * chunkWidth];
                    a = s;
                    s = d;
                    d = nextChunk[idx - chunkWidth + 1];
                    z = x;

                    x = getElement(w, a, s, d, z, ROW, COL);
                    nextChunk[idx] = x;

                    idx++;
                    ++COL;
                }
            }

            updatePrev(chunkCol);
        }

    }; // Builder

}; // SquareWallMorphism

//*/