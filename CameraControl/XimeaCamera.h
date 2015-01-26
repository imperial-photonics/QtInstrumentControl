
#pragma once
#include "AbstractStreamingCamera.h"
#include "ImageBuffer.h"

#include <QStringList>
/*
See
http://www.ximea.com/support/wiki/apis/XiApi_Manual
*/

class XimeaCamera : public AbstractStreamingCamera
{
public:

   static QStringList GetConnectedCameras();

   XimeaCamera(int camera_idx, QObject* parent = 0);
   ~XimeaCamera();

   int GetNumBytesPerPixel();
   cv::Size GetImageSize();
   int GetImageSizeBytes();
   int GetStride();
   double GetPixelSize();
   cv::Rect GetROI();

   void SetFullROI();
   void SetROI(cv::Rect roi);

   std::shared_ptr<ImageBuffer> GrabImage();

   void* GetHandle() { return xiH; }


   void GetImage(cv::Mat& cv_output);
   void SetIntegrationTime(int integration_time_ms_);

   void SetParameter(const QString& parameter, ParameterType type, QVariant value);
   QVariant GetParameter(const QString& parameter, ParameterType type);
   QVariant GetParameterLimit(const QString& parameter, ParameterType type, Limit limit);
   QVariant GetParameterMinIncrement(const QString& parameter, ParameterType type);
   EnumerationList GetEnumerationList(const QString& parameter);
   bool IsParameterWritable(const QString& parameter) { return true; }; // no algorithmic way to check 

   QWidget* GetControlWidget(QWidget* parent);

protected:

   void run();
   void FlushBuffers();


private:
   void* xiH;

   int integration_time_us = 100000;
   int timeout_ms = 1000;
};