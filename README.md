# 2026MURA

Here you will find several programs for generating number walls and automatic sequences, along with many example images of each.

## How to Build

```powershell
# Configure (run once or when adding new programs)
cmake -B build -S . -G "MinGW Makefiles"

# Build all programs
cmake --build build

# Build a specific program
cmake --build build --target [program name]
```

## Credits
stb_image.h and stb_image_write.h are by Sean Barrett, 2017.
This is a public domain library. See the file headers in Headers/stb_image.h and Headers/stb_image_write.h for full details.

Everything else is written by me (Luke Bennet) with inspiration (especially for p-Cantor related programs) from the paper:
"Fractals Emerging from the Toeplitz Determinants of the p-Cantor Sequence"
by Noy Soffer Aranov and Steven Robertson (2025)
arXiv:2510.19449
