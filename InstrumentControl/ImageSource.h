#pragma once

#include <cv.h>

/*
Generic interface describing an object which generates images
*/
class ImageSource
{
public:
   virtual cv::Mat GetImage() = 0;
   virtual cv::Mat GetNextImage() = 0;
};
