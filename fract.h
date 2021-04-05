#ifndef FRACT__H
#define FRACT__H

#include <complex>
#include <tuple>

#include <vector>
#include <chrono>
#include <functional>
#include <string>

#include <opencv2/core.hpp>


namespace FRACTAL
{
struct ZoomFrameHist
{
    ZoomFrameHist(
        const int frame_number_,
        const double x1_,
        const double x2_,
        const double y1_,
        const double y2_
    )
    : x1(x1_)
    , x2(x2_)
    , y1(y1_)
    , y2(y2_)
    , frame_number(frame_number_)
    {}
    int frame_number;
    double x1, x2, y1, y2;
    std::string info2file() const
    {
        return cv::format("%.15f %.15f %.15f %.15f", x1, x2, y1, y2);
    }
    std::string info() const
    {
        return cv::format(
            "frame::%d\nx1(%.15f),\nx2(%.15f),\ny1(%.15f),\ny2(%.15f);",
            frame_number,
            x1,
            x2,
            y1,
            y2);
    }
};

template <typename T>
//! @brief coordinate system 
class CS 
{
    T _x_min, _x_max, _y_min, _y_max;
public:
    CS(T x_min, T x_max, T y_min, T y_max)
    : _x_min(x_min), _x_max(x_max), _y_min(y_min), _y_max(y_max)
    {}
    T size() const {
        return (width() * height());
    }
    T width() const {
        return (_x_max - _x_min);
    }
    T height() const {
        return (_y_max - _y_min);
    }
    T x_min() const {
        return _x_min;
    }
    void x_min(T x_min) {
        _x_min = x_min;
    }	
    T x_max() const { return _x_max; }
    void x_max(T x_max) {  _x_max = x_max; }		
    T y_min() const { return _y_min; }
    void y_min(T y_min) { _y_min = y_min; }		
    T y_max() const { return _y_max; }
    void y_max(T y_max) { _y_max = y_max; }

    //! @brief reset coordinate system at new place
    void reset(T x_min, T x_max, T y_min, T y_max) 
    {
        _x_min = x_min;
        _x_max = x_max;
        _y_min = y_min;
        _y_max = y_max;
    }

    void reset(T x_mid, T y_mid, T R)
    {
        _x_min = x_mid - R;
        _x_max = x_mid + R;
        _y_min = y_mid - R;
        _y_max = y_mid + R;
    }

    std::pair<T, T> middle() const
    {
        return {
            _x_min + T(width()/2),
            _y_min + T(height()/2),
        };
    }

    cv::Rect rc() const
    {
        return cv::Rect(
            this->x_min(),
            this->y_min(),
            this->width(),
            this->height()
        );
    }

    static std::pair<double, double> rcMid(const cv::Rect& rc)
    {
        return std::make_pair<double, double>(
            rc.x + double(rc.width/2),
            rc.y + double(rc.height/2)
        );

    }
    
    static cv::Rect x1y1x2y2ToCvRect(
        const double x1,
        const double y1,
        const double x2,
        const double y2
    );

    void zoom(
        const double window_ratio, 
        const double x0, 
        const double x1,
        const double y0, 
        const double y1 
    );
    std::vector<ZoomFrameHist> zoom_history;
    std::string info() const;
};

struct CSHelper
{
    //! @brief convert a pixel coordinate to the complex domain
    static std::complex<double>  scale(
        CS<int> &scr, 
        CS<double> &fr, 
        std::complex<double>  c
    );

    template <typename FROM, typename TO>
    static std::pair<TO, TO> scale(
        CS<FROM> &scr, 
        CS<TO> &fr, 
        std::pair<FROM, FROM> c
    );
};

struct Fract
{
    Fract(
        const std::string& outDir_
    );
    std::string outDir = "";
    typedef std::complex<double> Complex;
    std::vector<ZoomFrameHist> readHistFromFile(const std::string& file_path);
    
    //! @brief loop over each pixel from our image and check 
    //         if the points associated with this pixel escape to infinity
    static void getNumberIterations(
        CS<int> &scr, 
        CS<double> &fract, 
        int iter_max, 
        std::vector<int> &colors,
        const std::function<std::complex<double>( 
            std::complex<double>, std::complex<double>
        )> &func);

    //! @brief check if a point is in the set or escapes to infinity, 
    //         return the number if iterations
    static int escape(
        Fract::Complex c, 
        int iter_max, 
        const std::function<Fract::Complex( 
            Fract::Complex, 
            Fract::Complex
        )> &func,
        double th=2.0
    );

    static cv::Mat computeFractal(
        CS<int> &scr, 
        CS<double> &fract, 
        int iter_max, 
        std::vector<int> &colors,
        const std::function<std::complex<double>( 
            std::complex<double>, std::complex<double>
        )> &func, 
        const char *fname, 
        bool smooth_color,
        const bool show=false,
        const bool write=true
    );

    cv::Mat vizOut(const cv::Mat& computed_fract);

    CS<double> mandelbrot(
        const cv::Point2d& x1y1,
        const cv::Point2d& x2y2,
        const int max_iter=500,
        const int outimg_w=1200, 
        const int outimg_h=1200,
        const bool show=false,
        const bool write=true
    );

    static cv::Mat plot(
        CS<int> &scr, 
        std::vector<int> &colors, 
        int iter_max, 
        const char *fname, 
        bool smooth_color,
        const bool show=false,
        const bool write=true
    );
};

} // namespace FRACT

#endif //FRACT__H