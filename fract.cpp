//#include <opencv2/core.hpp>
#include <iostream>
#include <vector>
#include <fstream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "fract.h"
#include "tools.h"

using namespace std;
using namespace FRACTAL;

string printRc(const cv::Rect& rc)
{
	return cv::format("(%d,%d) %dx%d", rc.x, rc.y, rc.width, rc.height);
}

FRACTAL::Fract::Fract(
	const std::string& outDir_
)
: outDir(outDir_)
{
	if(!isDirExist(this->outDir))
	{
		if(mkdir(this->outDir))
			cout << "Fract::created out dir::" << this->outDir << endl;
		else
			throw std::runtime_error("Fract::failed creating out dir::" + this->outDir);
	}
}


int FRACTAL::Fract::escape(
	Fract::Complex c, 
	int iter_max, 
	const std::function<Fract::Complex( 
		Fract::Complex, 
		Fract::Complex
	)> &func,
	double th
) 
{
	Fract::Complex z(0);
	int iter = 0;

	while (abs(z) < th && iter < iter_max) {
		z = func(z, c);
		iter++;
	}
	return iter;
}


void Fract::getNumberIterations(
	CS<int> &scr, 
	CS<double> &fract, 
	int iter_max, 
	std::vector<int> &colors,
	const std::function<Fract::Complex( 
		Fract::Complex, 
		Fract::Complex)> &func
) 
{
	cout << "process " << iter_max << " iters" << endl;
	int k = 0, progress = -1;
	const double th = 2.0;
	for(int i = scr.y_min(); i < scr.y_max(); ++i) {
		for(int j = scr.x_min(); j < scr.x_max(); ++j) {
			Complex c((double)j, (double)i);
			c = CSHelper::scale(scr, fract, c);
			colors[k] = escape(c, iter_max, func, th);
			k++;
		}
		if(progress < (int)(i*100.0/scr.y_max())){
			progress = (int)(i*100.0/scr.y_max());
			// std::cout << progress << '%' << std::flush;
			// std::cout.flush();
		}
	}
}

cv::Mat Fract::computeFractal(
  CS<int> &scr, 
  CS<double> &fract, 
  int iter_max, 
  std::vector<int> &colors,
  const std::function<Complex( Complex, Complex)> &func, 
  const char *fname, 
  bool smooth_color,
  const bool show,
  const bool write
) 
{
	cout << "computeFractal..." << endl;
	auto start = std::chrono::steady_clock::now();
	getNumberIterations(scr, fract, iter_max, colors, func);
	auto end = std::chrono::steady_clock::now();
	std::cout << "time to generate "
			  << fname << " = " 
			  << std::chrono::duration <double, std::milli> (end - start).count() 
			  << " [ms]" << std::endl;
	return Fract::plot(scr, colors, iter_max, fname, smooth_color, show, write);
}

CS<double> Fract::mandelbrot(
	const cv::Point2d& x1y1,
	const cv::Point2d& x2y2,
	const int max_iter,
	const int outimg_w, 
	const int outimg_h,
	const bool show,
	const bool write
)
{
	CS<int> scr(0, outimg_w, 0, outimg_h);
	CS<double> fract(x1y1.x, x2y2.x, x1y1.y, x2y2.y);
	//! @attention the function used to calculate the fractal
	// auto func = [] (Complex z, Complex c) -> Complex {return Complex(cos(45),cos(30))* z * z  + c; };
	auto func = [] (Complex z, Complex c) -> Complex {return z * z  + c; };
	string fname_pr = "mandelbrot.%03d.png";
	string histname_pr = "mandelbrot.fhistory";
	cv::Mat lastOut;
	auto hist_path = join(this->outDir, histname_pr);
	for(int i = 0; i < 1000; ++i)
	{
		string fname = cv::format(fname_pr.c_str(), i);
		auto f_path = join(this->outDir, fname);
		bool smooth_color = true;
		std::vector<int> colors(scr.size());
		cv::Rect targetBbox;
		bool zoomDone(false);
		cout << "zoom until press Esc..." << endl;
		double curx1(fract.x_min()),
			   curx2(fract.x_max()),
			   cury1(fract.y_min()),
			   cury2(fract.y_max());
		double newx1(fract.x_min()),
			   newx2(fract.x_max()),
			   newy1(fract.y_min()),
			   newy2(fract.y_max());
		auto viewer = Viewer(lastOut);
		if(show && ! lastOut.empty())
		{
			while(!zoomDone)
			{
				string window_name = "FRACT";
				auto viewer2draw = viewer.drawWithCursor();
				cv::imshow(window_name, viewer2draw);
				auto pressedKey = cv::waitKey(0);
				std::vector<Viewer::KeyboardKeys> commands;
				if(!Viewer::waitKey2Control(pressedKey, commands))
					zoomDone = true;
				// cout << "pressed::" << pressedKey << endl;
				viewer.moveByKey({static_cast<FRACTAL::Viewer::KeyboardKeys>(pressedKey)});
				viewer.moveTox1x2y1y2<double>(
					newx1,
					newx2,
					newy1,
					newy2
				);
				auto new_pt1 = CSHelper::scale<int, double>(scr, fract, {newx1, newy1});
				auto new_pt2 = CSHelper::scale<int, double>(scr, fract, {newx2, newy2});
				newx1 = new_pt1.first;
				newx2 = new_pt2.first;
				newy1 = new_pt1.second;
				newy2 = new_pt2.second;
				cout << "x1: " << newx1 << endl
					 << "y1: " << newy1 << endl
					 << "x2: " << newx2 << endl
					 << "y2: " << newy2 << endl;
				// bool showCrosshair = true;
				// bool fromCenter = false;
				// string sel_window = "FRACT::SelectRoi()";
				// cout << sel_window << endl;
				// targetBbox = cv::selectROI(sel_window, lastOut, showCrosshair, fromCenter);
				// if(targetBbox.width > targetBbox.height)
				// 	targetBbox.height = targetBbox.width;
				// else
				// 	targetBbox.width = targetBbox.height;
				// cv::destroyWindow(sel_window);
			}
		}

		if(targetBbox.area()>0)
		{
			auto new_pt1 = CSHelper::scale<int, double>(scr, fract, {targetBbox.x, targetBbox.y});
			auto new_pt2 = CSHelper::scale<int, double>(scr, fract, {
				targetBbox.x + targetBbox.width, 
				targetBbox.y + targetBbox.height
			});
			newx1 = new_pt1.first;
			newx2 = new_pt2.first;
			newy1 = new_pt1.second;
			newy2 = new_pt2.second;
		}

		fract.zoom(
			1.0, 
			newx1, 
			newx2, 
			newy1, 
			newy2);
		cout << "HISTORY" << endl;
		std::vector<std::string> str_history;
		ofstream f(hist_path);
		for(const auto& fz: fract.zoom_history)
		{
			cout << fz.info() << endl;
			f << fz.info2file() << '\n';
		}
		cout << "written to " << hist_path << endl;
		auto midFract = fract.middle();
		f.close();
		cout << fract.info() << endl;
			cout << "ti che" << endl;
		lastOut = computeFractal(scr, fract, max_iter, colors, func, f_path.c_str(), smooth_color, show, write);
		if(!lastOut.empty())
			cout << "done" << endl;
	}
	return fract;
}

cv::Mat vizOut(const cv::Mat& computed_fract)
{
	
}

std::tuple<int, int, int> get_rgb_piecewise_linear(int n, int iter_max) {
	int N = 256; // colors per element
	int N3 = N * N * N;
	// map n on the 0..1 interval (real numbers)
	double t = (double)n/(double)iter_max;
	// expand n on the 0 .. 256^3 interval (integers)
	n = (int)(t * (double) N3);
	int b = n/(N * N);
	int nn = n - b * N * N;
	int r = nn/N;
	int g = nn - r * N;
	return std::tuple<int, int, int>(r, g, b);
}

std::tuple<int, int, int> iters2rgbBernstein(int n, int iter_max) 
{
	// map n on the 0..1 interval
	double t = (double)n/(double)iter_max;
	int r = (int)(4*(1-t*t)*t*255);
	int g = (int)(15*(1-t)*(1-t)*t*t*255);
	int b =  (int)(8.5*(1-t)*(1-t)*(1-t)*t*255);	
	return std::tuple<int, int, int>(r, g, b);
}


cv::Mat Fract::plot(
	CS<int> &scr, 
	std::vector<int> &colors, 
	int iter_max, 
	const char *fname, 
	bool smooth_color,
	const bool show,
	const bool write
) 
{
	unsigned int width = scr.width(), height = scr.height();
	cv::Mat bitmap(width, height, CV_8UC3);
	int k = 0;
	std::tuple<int, int, int> rgb;
	for(int i = scr.y_min(); i < scr.y_max(); ++i) {
		for(int j = scr.x_min(); j < scr.x_max(); ++j) {
			int n = colors[k];
			if( !smooth_color ) {
				rgb = get_rgb_piecewise_linear(n, iter_max);
			}
			else {
				rgb = iters2rgbBernstein(n, iter_max);
			}
			// or revert indxs?
			cv::Scalar col_bgr {
				static_cast<double>(std::get<2>(rgb)),
				static_cast<double>(std::get<1>(rgb)),
				static_cast<double>(std::get<0>(rgb)),
			};

			cv::circle(
				bitmap,
				{j,i},
				1,
				col_bgr,
				1
			);
			k++;
		}
	}
	if(write)
	{
		cv::imwrite(fname, bitmap);
		cout << "written at " << fname << endl;
	}
	// if(show)
	// {
	// 	cv::imshow("FRACT::show()", bitmap);
	// 	cv::waitKey(0);
	// }
	return bitmap;
}

std::vector<ZoomFrameHist> Fract::readHistFromFile(const std::string& file_path)
{
	std::vector<ZoomFrameHist> out;
	ifstream f(file_path);
	double x1, x2, y1, y2;
	int i = -1;
	while (f >> x1 >> x2 >> y1 >> y2)
	{
		++i;
		out.emplace_back(i, x1, x2, y1, y2);
	}
	for (const auto& c: out)
	{
		cout << c.info() << endl;
	}
	f.close();
	return out;
}

// CS
template <typename T>
void FRACTAL::CS<T>::zoom(
	const double window_ratio, 
	const double x0, 
	const double x1,
	const double y0, 
	const double y1
) 
{
	double y = (x1 - x0) * window_ratio;
	this->reset(x0, x1, y0, y + y0);
	if(this->zoom_history.empty())
	{
		zoom_history.emplace_back(0,x0,x1,y0,y1);
		return;
	}
	auto last_z = zoom_history.back();
	zoom_history.emplace_back(last_z.frame_number+1,x0,x1,y0,y1);
}

template <typename T>
std::string FRACTAL::CS<T>::info() const
{
	auto mid = this->middle();
	std::string info = cv::format(
		"CS info::\nCSMid::(%.8f,%.8f)\nwh::(%.8f, %.8f)",
		mid.first,
		mid.second,
		this->width(),
		this->height()
	);
	return info;
}


std::complex<double>  FRACTAL::CSHelper::scale(
	CS<int> &scr, 
	CS<double> &fr, 
	std::complex<double> c
) 
{
	std::complex<double> aux(
		c.real() / (double)scr.width() * fr.width() + fr.x_min(),
		c.imag() / (double)scr.height() * fr.height() + fr.y_min()
	);
	return aux;
}

template <typename FROM, typename TO>
std::pair<TO, TO> FRACTAL::CSHelper::scale(
	CS<FROM> &scr, 
	CS<TO> &fr, 
	std::pair<FROM, FROM> c
) 
{
	return std::make_pair<TO, TO>(
		c.first / (TO)scr.width() * fr.width() + fr.x_min(),
		c.second / (TO)scr.height() * fr.height() + fr.y_min()
	);
}

bool FRACTAL::Viewer::waitKey2Control(
        const int k,
        std::vector<FRACTAL::Viewer::KeyboardKeys>& commands
)
{
	if(k == Viewer::KeyboardKeys::ARROW_UP) // arrow up 
	{
		commands.push_back(Viewer::KeyboardKeys::ARROW_UP);
	}
	else if(k == Viewer::KeyboardKeys::ARROW_LEFT) // arrow left 
	{
		commands.push_back(Viewer::KeyboardKeys::ARROW_LEFT);
	}
	else if(k == Viewer::KeyboardKeys::ARROW_RIGHT) // arrow right 
	{
		commands.push_back(Viewer::KeyboardKeys::ARROW_RIGHT);
	}
	else if(k == Viewer::KeyboardKeys::ARROW_DOWN) // arrow right 
	{
		commands.push_back(Viewer::KeyboardKeys::ARROW_DOWN);
	}
	else if(k == 122) // z
	{

	}
	else if(k == 120) // x
	{
	}
	else if(k == 27) // Esc
	{
		std::cout << "Esc pressed..." << std::endl;
		return false;
	}
	return true;
}

cv::Mat FRACTAL::Viewer::drawWithCursor() const
{
	auto img2draw = this->src2view.clone();
	auto half_line_width = int(src2view.size().width/this->xMod);
	auto half_line_height = int(src2view.size().width/this->xMod);
	try{
		cv::line(
			img2draw,
			{
				this->xCurrent-half_line_width,
				this->yCurrent-half_line_height
			},
			{
				this->xCurrent+half_line_width,
				this->yCurrent+half_line_height
			},
			{
				0,255,0
			},
			2
		);
		cv::line(
			img2draw,
			{
				this->xCurrent+half_line_width,
				this->yCurrent-half_line_height
			},
			{
				this->xCurrent-half_line_width,
				this->yCurrent+half_line_height
			},
			{
				255,0,255
			},
			2
		);
	}
	catch(...)
	{}
	return img2draw;
}

void FRACTAL::Viewer::moveByKey(const std::vector<Viewer::KeyboardKeys>& keys)
{
	if(keys.empty())
		return;
	if(std::find(keys.begin(), keys.end(), Viewer::KeyboardKeys::ARROW_UP) != keys.end())
	{
		cout << "Viewer::KeyboardKeys::ARROW_UP" << endl;
		this->yCurrent -= this->yStep;
		if(this->yCurrent < 0)
			this->yCurrent = 0;
	}
	else if(std::find(keys.begin(), keys.end(), Viewer::KeyboardKeys::ARROW_DOWN) != keys.end())
	{
		cout << "Viewer::KeyboardKeys::ARROW_DOWN" << endl;
		this->yCurrent += this->yStep;
		if(this->yCurrent > this->src2view.size().height)
			this->yCurrent = this->src2view.size().height;
	}
	else if(std::find(keys.begin(), keys.end(), Viewer::KeyboardKeys::ARROW_LEFT) != keys.end())
	{
		cout << "Viewer::KeyboardKeys::ARROW_LEFT" << endl;
		this->xCurrent -= this->xStep;
		if(this->xCurrent < 0)
			this->xCurrent = 0;
	}
	else if(std::find(keys.begin(), keys.end(), Viewer::KeyboardKeys::ARROW_RIGHT) != keys.end())
	{
		cout << "Viewer::KeyboardKeys::ARROW_RIGHT" << endl;
		this->xCurrent += this->xStep;
		if(this->xCurrent > this->src2view.size().width)
			this->xCurrent = this->src2view.size().width;
	}
	else if(std::find(keys.begin(), keys.end(), Viewer::KeyboardKeys::ZUM_IN) != keys.end())
	{
		cout << "Viewer::KeyboardKeys::ZUM_IN" << endl;
		this->xMod += 1;
		this->yMod += 1;
	}
	else if(std::find(keys.begin(), keys.end(), Viewer::KeyboardKeys::ZUM_OUT) != keys.end())
	{
		cout << "Viewer::KeyboardKeys::ZUM_OUT" << endl;
		this->xMod -= 1;
		this->yMod -= 1;
	}
}

template <typename T>
void FRACTAL::Viewer::moveTox1x2y1y2(
        T &x1,
        T &x2,
        T &y1,
        T &y2
)
{
	auto half_line_width = int(src2view.size().width/this->xMod);
	auto half_line_height = int(src2view.size().width/this->xMod);
	x1 = T(this->xCurrent-half_line_width);
	x2 = T(this->xCurrent+half_line_width);
	y1 = T(this->yCurrent-half_line_height);
	y2 = T(this->yCurrent+half_line_height);
}