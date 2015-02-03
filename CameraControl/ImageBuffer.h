#pragma once

#include <cv.h>

class AbstractStreamingCamera;

/*
An image buffer wrapper for AbstractStreamingCamera
*/

class ImageBuffer
{
public:
   ImageBuffer();
   ImageBuffer(cv::Mat image, cv::Mat& background, AbstractStreamingCamera* camera, int image_index);

   cv::Mat& GetImage();
   cv::Mat& GetBackgroundSubtractedImage();

   ~ImageBuffer();

private:
   AbstractStreamingCamera* camera;
   cv::Mat image;
   cv::Mat background_subtracted;
   bool is_null = true;
   int allocation_idx = -1;
   int image_index = 0;
};

typedef ImageBuffer Buf; // temporary while refactoring
