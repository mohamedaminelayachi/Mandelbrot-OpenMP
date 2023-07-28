# Mandelbrot-OpenMP
A parallelized program to visualize the Mandelbrot set, built with C++, OpenMP, and SDL2.

This program was implemented using C++, together with the OpenMP library, and it also uses the SDL2 library to render the computations on the screen which will eventually, allow the Mandelbrot set to be visualized; therefore, to run this program, you must have:
1. OpenMP (The compiler used is TDM-G++ 64bit).
2. SDL 2.
3. C++ 20

### To compile the file, use the following command:
- g++ -fopenmp -std=c++20 Mandelbrot.cpp -o Mandelbrot -lSDL2

In 1.581 seconds, the program was able to render out the Mandelbrot set by running over 1000 iterations. (AMD Ryzen 7 5800H Octa-Core)

