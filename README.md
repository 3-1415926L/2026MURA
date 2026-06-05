

Note, I did not write the stb_image_write.h file. Everything else is written by me with a few algorithms based on equations by Steven Robertson.



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
## 
stb_image.h and stb_image_write.h by Sean Barrett, 2017.
This is a public domain library. See the file headers in Headers/stb_image.h and Headers/stb_image_write.h for full details.