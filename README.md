# 2026MURA

Here you will find several programs for generating number walls and automatic sequences, along with many example images of each.

## How to Build (cmake)
Use this if you have cmake installed

```powershell
# Configure (run once or when adding new programs/headers)
cmake -B build -S . -G "MinGW Makefiles"

# Build all programs
cmake --build build

# Build a specific program
cmake --build build --target [program name]
```

## How to Build (make)
Use this if you have make installed and are using Linux or Mac

```powershell
# Build all programs
make

# Build a specific program
make [program name]
```

## Program Information
### AutoSeqGenerator
This program takes in a 2D morphism and outputs an image of the result of that morphism iterated a given number of times. 

To use: Fill in inputAutomaticSequences.txt with the desired morphism and parameters and run the executable.

### CantorToMorphism
This program takes a sequence and attempts to find a morphism that generates the number wall of the sequence.

To use: Run the executable and follow the input prompts.

### NWGenerator
This program reads a sequence from a specified sequence file (in OEIS format, see sequences folder for examples) and generates an image of that sequence's number wall.

To use: Fill in inputNumberwalls.txt with the desired sequence file and parameters and run the executable.
(You may also create your own sequence file. If you do so, make sure to place it in the sequences folder.)

### pagoda
This program checks if there are any zero windows of size 2+ anywhere in the first 10,000 columns of the Pagoda sequence's number wall mod p, where p ranges over all primes of the form 4k-1. (For p > 1000, integer overflow may cause false positives)

To use: Run the executable.

## Notes
The (p,k)-Cantor sequence is the morphism that sends 1 to the coefficients of (1+x^2+...+x^(2k))^((p-1)/4) (mod p), and sends every other number n to n times the image of 1 (mod p). This is a generalization of the p-Cantor sequence defined by Noy Soffer Aranov and Steven Robertson. (Of note, the p-Cantor sequence equals the (p,1)-Cantor sequence.)

## Credits
stb_image.h and stb_image_write.h are by Sean Barrett, 2017.
This is a public domain library. See the file headers in Headers/stb_image.h and Headers/stb_image_write.h for full details.

Everything else is written by me (Luke Bennet) with inspiration (especially for p-Cantor related programs) from the paper:
"Fractals Emerging from the Toeplitz Determinants of the p-Cantor Sequence"
by Noy Soffer Aranov and Steven Robertson (2025)
arXiv:2510.19449
