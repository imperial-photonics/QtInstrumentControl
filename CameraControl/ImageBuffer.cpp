#include "ImageBuffer.h"
#include "AbstractStreamingCamera.h"


ImageBuffer::ImageBuffer()
{
   image = cv::Mat::zeros(128, 128, CV_8U);
}

ImageBuffer::ImageBuffer(cv::Mat image, cv::Mat& background, AbstractStreamingCamera* camera, int image_index) :
   camera(camera), 
   image(image),
   image_index(image_index),
   is_null(false)
{
   allocation_idx = camera->allocation_idx;

   if (background.size() == image.size())
      background_subtracted = image - background;
   else
      background_subtracted = image;
}

cv::Mat& ImageBuffer::GetImage()
{
   if (!is_null && (camera->allocation_idx != allocation_idx))
      image = cv::Mat::zeros(128, 128, CV_8U);
   return image;
}

cv::Mat& ImageBuffer::GetBackgroundSubtractedImage()
{
   if (!is_null && (camera->allocation_idx != allocation_idx))
      image = cv::Mat::zeros(128, 128, CV_8U);

   return background_subtracted;
}

ImageBuffer::~ImageBuffer()
{
   if (!is_null && (camera->allocation_idx == allocation_idx))
      camera->QueuePointer(image.data);
}