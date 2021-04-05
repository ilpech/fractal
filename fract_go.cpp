#include <iostream>

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
	// double min_x(0.095),
	// 	   min_y(0.585),
	// 	   max_x(0.155),
	// 	   max_y(0.645);

// fract.x_min()::0.100002
// fract.y_min()::0.614558
// fract.x_max()::0.100968
// fract.y_max()::0.615524

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
	int max_iter(600);
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