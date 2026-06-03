import sys

sys.stdout = open(R"C:\Users\HP\Git Repositories\2026MURA\output.txt", "w")

rotateSpecial = {
    "0" : "0",
    "F" : "F",
    "CNE" : "CSE",
    "CNW" : "CNE",
    "CSE" : "CSW",
    "CSW" : "CNW",
    "C1" : "C2",
    "C2" : "C1",
    "D1" : "D2",
    "D2" : "D1",
    "W3" : "W4",
    "W4" : "W3",
    "W10" : "W11",
    "W11" : "W10",
    "H2" : "H3",
    "H3" : "H2"
}

def rotate(symbol): # 90 degrees clockwise
    if symbol in rotateSpecial:
        return rotateSpecial[symbol]
    base = symbol[0]
    symbol = symbol[1:]
    result = ""
    for c in symbol:
        if c == "E":
            c = "S"
        elif c == "N":
            c = "E"
        elif c == "W":
            c = "N"
        elif c == "S":
            c = "W"
        elif c == "V":
            c = "H"
        elif c == "H":
            c = "V"
        elif c == "P":
            c = "R"
        elif c == "R":
            c = "P"
        elif c == "Q":
            c = "T"
        elif c == "T":
            c = "Q"
    return base + result

def rotateGrid(grid):
    h = len(grid)
    w = len(grid[0])
    result = []
    for r in range(h):
        result.append([0] * w)

    for r in range(h):
        for c in range(w):
            result[c][h - 1 - r] = rotate(grid[r][c])
    return result

def reflect(symbol): # swaps top and bottom
    base = symbol[0]
    symbol = symbol[1:]
    result = ""
    for c in symbol:
        if c == "N":
            c = "S"
        elif c == "S":
            c = "N"
        elif c == "C":
            c = "A"
        elif c == "A":
            c = "C"
        elif c == "P":
            c = "Q"
        elif c == "Q":
            c = "P"
        elif c == "R":
            c = "T"
        elif c == "T":
            c = "R"
    return base + result

def reflectGrid(grid):
    h = len(grid)
    w = len(grid[0])
    result = []
    for r in range(h):
        result.append([0] * w)

    for r in range(h):
        for c in range(w):
            result[h - 1 - r][c] = reflect(grid[r][c])
    return result

def printGrid(symbol, grid):
    print(symbol)
    h = len(grid)
    for r in range(h):
        print(" ".join(grid[r]))
    print("")



symbolList = ["0", "F"]

print("0\n", \
      "0 0 0 0 0\n", \
      "0 0 0 0 0\n", \
      "0 0 0 0 0\n", \
      "0 0 0 0 0\n", \
      "0 0 0 0 0\n", sep="")
print("F\n", \
      "CNW EN EN EN CNE\n", \
      "EW 0 0 0 EE\n", \
      "EW 0 0 0 EE\n", \
      "EW 0 0 0 EE\n", \
      "CSW ES ES ES CSE\n", sep="")

with open(R"C:\Users\HP\Git Repositories\2026MURA\temp.txt", "r") as file:
    lines = file.readlines() # \n at end of each
    i = 0

    while i < len(lines):
        symbol = lines[i].strip()
        i += 1
        if symbol == "":
            continue
        symbol = symbol.replace(":","")

        grid = []
        for row in range(5):
            grid.append([])
            for col in range(5):
                x = lines[i + 5 * row + col].strip()
                grid[row].append(x)

        i += 25
        symbolList.append(symbol)
        printGrid(symbol, grid)

        if symbol in ["CNE", "CNW", "CSE", "CSW"]:
            for loop in range(3):
                symbol = rotate(symbol)
                symbolList.append(symbol)
                grid = rotateGrid(grid)
                printGrid(symbol, grid)
            continue

        # [1:] is used because "W" and "E" are also symbol types
        numDirections = ("N" in symbol[1:]) + ("E" in symbol[1:]) \
                      + ("S" in symbol[1:]) + ("W" in symbol[1:])
        
        if numDirections == 0:
            symbol = rotateSpecial[symbol]
            symbolList.append(symbol)
            grid = rotateGrid(grid)
            printGrid(symbol, grid)
            continue

        if numDirections == 1:
            for loop in range(3):
                symbol = rotate(symbol)
                symbolList.append(symbol)
                grid = rotateGrid(grid)
                printGrid(symbol, grid)
            continue

        if numDirections == 2:
            for loop in range(3):
                symbol = rotate(symbol)
                symbolList.append(symbol)
                grid = rotateGrid(grid)
                printGrid(symbol, grid)
            symbol = rotate(symbol) # back to original
            grid = rotateGrid(grid)

            symbol = reflect(symbol) # reflected
            symbolList.append(symbol)
            grid = reflectGrid(grid)
            printGrid(symbol, grid)

            for loop in range(3): # rotated reflections
                symbol = rotate(symbol)
                symbolList.append(symbol)
                grid = rotateGrid(grid)
                printGrid(symbol, grid)
            continue

        print("Error! Should not have gotten to this point.")
        
print("\n\nCoding\n\n0 0\n")
for s in symbolList[1:]:
    print(s, "1\n")

print("\n\nlen =",len(symbolList))

sys.stdout.close()