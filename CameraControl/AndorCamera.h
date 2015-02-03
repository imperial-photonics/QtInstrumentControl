#pragma once

#define NOMINMAX

#include "AbstractStreamingCamera.h"
#include "ImageBuffer.h"
#include "atcore.h"

class AndorCamera : public AbstractStreamingCamera
{
   Q_OBJECT

public:

   AndorCamera(int idx = 0, QObject* parent = 0);
   ~AndorCamera();

   static void GetConnectedCameras(QStringList& camera_list, QVector<int>& camera_idx);
   
   int    GetNumBytesPerPixel();
   cv::Size  GetImageSize();
   int    GetImageSizeBytes();
   int    GetStride();
   double GetPixelSize();
   AT_H   GetHandle();
   cv::Rect GetROI();

   void   SetROI(cv::Rect roi);
   void   SetFullROI();
   void SetUseExternalTriggering(bool use_external_triggering);

   void SetParameter(const QString& parameter, ParameterType type, QVariant value);
   QVariant GetParameter(const QString& parameter, ParameterType type);
   QVariant GetParameterLimit(const QString& parameter, ParameterType type, Limit limit);
   QVariant GetParameterMinIncrement(const QString& parameter, ParameterType type);

   EnumerationList GetEnumerationList(const QString& parameter);
   bool IsParameterWritable(const QString& parameter);
   bool IsParameterReadOnly(const QString& parameter);

   std::shared_ptr<ImageBuffer> GrabImage();

   QWidget* GetControlWidget(QWidget* parent = 0);

protected:

   void run();

private:
   void QueuePointerWithCamera(AT_U8* ptr);
   void FlushBuffers();
   void Callback(const AT_WC* feature);

   QSize current_size;
   int current_stride;

   AT_H Hndl;

   friend int AT_EXP_CONV AndorFeatureCallback(AT_H Hndl, const AT_WC* feature, void* context);
};
