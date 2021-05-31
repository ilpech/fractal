#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
 

#include "tools.h"
#include "fract.h"

bool isPtInImg(const cv::Mat& img, const cv::Point2d& pt)
{
    if(pt.x < 0 || pt.x > img.size().width)
        return false;
    if(pt.y < 0 || pt.y > img.size().height)
        return false;
    return true;
}

void drawArrow(
    cv::Mat& img, 
    const cv::Point2d& pt_from, 
    const cv::Point2d& pt_to,
    const cv::Scalar& clr,
    const int thickness
)
{
    if(img.empty())
        return;
    cv::line(
        img,
        pt_from,
        pt_to,
        clr,
        thickness
    );
    cv::line(
        img,
        pt_to,
        {
            int((pt_from.x + pt_to.x)/2),
            int((pt_from.y + pt_to.y)/2)
        },
        clr,
        thickness
    );
    cv::line(
        img,
        pt_to,
        {
            int((pt_from.x - pt_to.x)/2),
            int((pt_from.y - pt_to.y)/2)
        },
        clr,
        thickness
    );
}

int main()
{
    cv::Mat blank(900, 900, CV_8UC3, {0,0,0});
    drawArrow(blank, {100,100}, {150,100}, {255,255,255}, 1);
    cv::imshow("img", blank);
    cv::waitKey(0);

    return 0;
}