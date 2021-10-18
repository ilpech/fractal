#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "tools.h"
#include "fract.h"

using namespace std;



int main() 
{
	std::string dir;
#ifdef __linux__
	dir = "/datasets/tests/fract";
#else
	dir = "/Users/ilpech/datasets/tests/fract";
#endif
	std::string out(FRACTAL::join(
			dir,
			FRACTAL::currentDateTime()
	));
	FRACTAL::Fract fractal(out);
	double 
// x1(-0.562202623667693),
// x2(-0.562202612966235),
// y1(-0.642817157614104),
// y2(-0.642817146912646);

//petlya
// x1(-0.748691950590000),
// x2(-0.748689956490000),
// y1(-0.084454610590000),
// y2(-0.084452616490000);
		   x1(-2.2),
		   x2(1.2),
		   y1(-1.7),
		   y2(1.7);

// x1(-1.777349275511142),
// x2(-1.777349217972738),
// y1(0.001270663932978),
// y2(0.001270721471382);

	int w_out(2000),
		h_out(2000);
	// int max_iter(100);
	// int max_iter(500);
	int max_iter(200);
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
