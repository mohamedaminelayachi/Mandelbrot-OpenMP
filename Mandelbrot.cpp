
#include <iostream>
#include <complex>
#include <omp.h>
#include <numeric>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

/*
	AUTHOR: MOHAMED AMINE LAYACHI
	DATE: 05/04/2023
	LANGUAGE: C++
	COMPILER: TDM-G++
*/

const int HEIGHT = 400;
const int WIDTH = 400;


/* The Renderer Class is used for everything the screen displays, it contains methods that use the SDL libray
to set up a window where the Mandelbrot visualization can be achieved.*/

class Renderer {
public:
	/*
		The initialization of a Renderer object takes no arguments, and it sets up every thing that the SDL 
		library need to make properly a graphics rendering pipeline which will make use of the computations
		done in the Mandelbrot class and draw graphics on screen.
	*/
	Renderer() {
		main_renderer = nullptr; main_window = nullptr;
		SDL_Init(SDL_INIT_EVERYTHING);
		SDL_CreateWindowAndRenderer(2*WIDTH, 2*HEIGHT, 0, &main_window, &main_renderer);
		SDL_RenderSetLogicalSize(main_renderer, WIDTH, HEIGHT);
		SDL_RenderSetScale(main_renderer, 2, 2);
	}
	/*
		getRenderer returns the main_renderer attribute's reference.
	*/
	SDL_Renderer* getRenderer() const {
		return main_renderer;
	}
	/*
		getWindow returns the main_window attibute's reference
	*/
	SDL_Window* getWindow() const{
		return main_window;
	}

	/*
		getEvent returns the main_event attribute.
	*/
	SDL_Event& getEvent(){
		return main_event;
	}
	/*
		Run uses a method in the SDL library to 'turn on' the renderer.
	*/
	void Run() {
		SDL_RenderPresent(main_renderer);
	}

private:
	SDL_Window* main_window;
	SDL_Renderer* main_renderer;
	SDL_Event main_event;
};

/*  The Mandelbrot class is used for computing the mandelbrot set points, it contains methods that use the OpenMP Library
	clauses to parallelize the computation on multiple processors (if detected). */
class Mandelbrot{
public:
	/*
		Mandelbrot initialization relies on two main parameters:
		-- iterations : the number of iterations that will be used to decide if a point is inside the set or not.
		-- starting_point : the point from which the search will start, used for experimentation purposes the user can try
							different starting points, which will give different results. it is set to 0 by default.
	*/
	Mandelbrot(int iterations, std::complex<double> starting_point = std::complex<double>(0,0)){
		this->iterations = iterations;
		this->starting_point = starting_point;
		threshold = 4;
		min_lerp = -2.0;
		max_lerp = 2.0;
		running = false;
	}
	
	/*  Removal of the default copy constructor and the assignment operator to avoid any external issues. And because
		they are unnecessary neither for this particular project nor at the moment at least.*/
	Mandelbrot(const Mandelbrot&) = delete;
	Mandelbrot& operator= (const Mandelbrot&) = delete;

	/*
		setExplicitThreshold is used to set an explicit threshold under which the divergence will hold.
	*/
	void setExplicitThreshold(double threshold){
		this->threshold = threshold;
	}

	/*
		setIterations is used to reset the number of iterations.
	*/
	void setIterations(int iterations) {
		this->iterations = iterations;
	}

	/*
		inside_the_mandelbrot is the method used to decide whether a complex number (Passed as an argument) is inside 
		the Mandelbrot set or not.
	*/
	int inside_the_mandelbrot(std::complex<double> complexNumber){
		std::complex<double> Z = starting_point;
		int i = 0;
		
		while(i < iterations && std::norm(Z) <= threshold){
			Z = Z * Z + complexNumber;
			
			if ((i+1) == iterations) {
				return i; // Returned for a coloring aspect. 
			}
			i++;
		}
		return 0;
	}
	
	/*
		visualize is used to visualize the Mandelbrot set by iterating over all the linearly interpolated points up to 
		the number of iterations set.
	*/
	void visualize() {

		double start = omp_get_wtime();

		#pragma omp parallel for
		for (int i = 0; i < iterations; i++) {
			#pragma omp parallel for
			for (int j = 0; j < iterations; j++) {
				double point_a = (double)i / (double)iterations;
				double point_b = (double)j / (double)iterations;
				double x = std::lerp(min_lerp, max_lerp, point_a); // The visualization is bounded from above by |2|
				double y = std::lerp(min_lerp, max_lerp, point_b);
				int temp_iterations = inside_the_mandelbrot(std::complex<double>(x, y));
				if (temp_iterations == 0) {
					// Convergence Logic
					#pragma omp critical
					{
						SDL_SetRenderDrawColor(artist.getRenderer(), 0, 0, 0, 255);
						SDL_RenderDrawPointF(artist.getRenderer(), point_a * WIDTH, point_b * HEIGHT);
					}
				}
				else {
					// Divergence Logic - Uniform Coloring
					#pragma omp critical
					{
						SDL_SetRenderDrawColor(artist.getRenderer(),
							245 * temp_iterations % 255,
							245 * temp_iterations % 255,
							245 * temp_iterations % 255,
							255);
						SDL_RenderDrawPointF(artist.getRenderer(), point_a * WIDTH, point_b * HEIGHT);
					}
				}
			}
		}

		artist.Run();

		time_log = omp_get_wtime() - start;

		// Closing Event
		running = true;
		while (running) {
			while (SDL_PollEvent(&artist.getEvent()) != 0) {
				if (artist.getEvent().type == SDL_QUIT) {
					running = false;
				}
			}
		}
		SDL_DestroyWindow(artist.getWindow());
		SDL_Quit();
		
	}
	/*
		get_time_log is used to get the time taken to do the computation of the set and render everything on screen.
	*/
	void get_time_log() {
		std::cout << "Timing: " << time_log << "s" << std::endl;
	}
	
private:
	int iterations;
	double min_lerp;
	double max_lerp;
	std::complex<double> starting_point;
	double threshold;
	Renderer artist;
	bool running;
	double time_log;
};

int main(){
	
	Mandelbrot M(1000, std::complex<double>(0,0));
	M.visualize();
	M.get_time_log();
	return 0;
}