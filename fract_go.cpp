#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "tools.h"
#include "fract.h"

using namespace std;



int main() 
{
	std::string dir("/Users/ilpech/datasets/tests/fract");
	std::string out(FRACTAL::join(
			dir,
			FRACTAL::currentDateTime()
	));
	FRACTAL::Fract fractal(out);
	double 
		   x1(-2.2),
		   x2(1.2),
		   y1(-1.7),
		   y2(1.7);
		// x1(0.415712345679013),
		// x2(0.416230740740741),
		// y1(0.618266913580247),
		// y2(0.618785308641976);
	int w_out(900),
		h_out(900);
	// int max_iter(100);
	// int max_iter(500);
	int max_iter(150);
	const bool show=true;
	const bool write=true;
	fractal.mandelbrot(
		{x1, y1},
		{x2, y2},
		max_iter,
		w_out, 
		h_out,
		show,
		write
	);
	return 0;
}