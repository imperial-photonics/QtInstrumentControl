#pragma once

#include "ImageSource.h"
#include "ImageBuffer.h"

#include <QThread>
#include <QMutex>
#include <QSize>
#include <QWaitCondition>
#include <QVariant>

#include <stdint.h>
#include <cassert>

#include <memory>
#include <vector>

#include <cv.h>

void CHECK(int err);
void SOFTCHECK(int err);


enum ParameterType { Integer, Float, Boolean, Text, Enumeration };
enum Limit { Min, Max };

typedef ParameterType ControlType; // refactoring

typedef QList<QPair<QString, int>> EnumerationList;

class AbstractStreamingCamera : public QObject, public ImageSource
{
   Q_OBJECT

public:
   AbstractStreamingCamera(QObject* parent = 0);
   ~AbstractStreamingCamera();

   virtual int GetNumBytesPerPixel() = 0;
   virtual cv::Size GetImageSize() = 0;
   virtual int GetImageSizeBytes() = 0;
   virtual int GetStride() = 0;
   virtual double GetPixelSize() = 0;
   virtual cv::Rect GetROI() = 0;

   virtual void SetParameter(const QString& parameter, ParameterType type, QVariant value) = 0;
   virtual QVariant GetParameter(const QString& parameter, ParameterType type) = 0;
   virtual QVariant GetParameterLimit(const QString& parameter, ParameterType type, Limit limit) = 0;
   virtual QVariant GetParameterMinIncrement(const QString& parameter, ParameterType type) { return QVariant(); }; // returns QVariant() if no min increment
   virtual EnumerationList GetEnumerationList(const QString& parameter) = 0;
   virtual bool IsParameterWritable(const QString& parameter) { return true; };
   virtual bool IsParameterReadOnly(const QString& parameter) { return false; };

   virtual void SetFullROI() = 0;
   virtual void SetROI(cv::Rect roi) = 0;

   virtual std::shared_ptr<ImageBuffer> GrabImage() = 0;

   virtual QWidget* GetControlWidget(QWidget* parent = 0) = 0;

   void UpdateBackground();
   void ClearBackground();

   void SetStreamingStatus(bool streaming);
   std::shared_ptr<ImageBuffer> GetLatest();
   std::shared_ptr<ImageBuffer> GetNext();

   cv::Mat GetNextImage();
   cv::Mat GetImage();
   cv::Mat BackgroundImage();

   QMutex* control_mutex;

signals:
   void ImageSizeChanged();
   void NewImage();
   void NewBackground();
   void StreamingStatusChanged(bool is_streaming);
   void StreamingFinished();
   void ControlLockUpdated(bool locked);

protected:

   /*
      run() will be executed in a new thread. 

      It should start the camera streaming and continuously request new images in a loop
      that terminates when the variable terminate == true. 

      When it has a new image, it should call SetLatest() with the new data.

      When the loop returns it should call TerminateStreaming()
   */
   virtual void run() = 0;

   /*
      QueuePointer() should return a pointer to the camera buffer.
      This will be called when the program has finished processing an image.
   */
   virtual void QueuePointerWithCamera(unsigned char* ptr) {};

   /*
      FlushBuffer() should flush the buffer
   */
   virtual void FlushBuffers() = 0;

   void AllocateBuffers(int buffer_size);
   void QueueAllBuffers();
   void SetLatest(cv::Mat& image);
   void TerminateStreaming();

   unsigned char* GetUnusedBuffer();

   void FreeBuffers();
   
   const static int n_buffers = 5; 
   int buffer_size;
   int max_buffer_size;

   bool is_streaming;
   bool terminate;
   QThread* main_thread;
   QThread* worker_thread;

   QWaitCondition next_cv;
   QMutex* next_mutex;

   //cv::Size image_size;
   //int bytes_per_pixel;
   //int stride;

   bool controls_locked = false;

private:

   void QueuePointer(unsigned char* ptr);

   float* background_ptr;
   cv::Mat background;

   bool init;
   bool buffers_ok;
   int  allocation_idx;

   std::vector<unsigned char*> buffers;
   std::list<unsigned char*> unused_buffers;
   std::shared_ptr<ImageBuffer> latest_data;

   QMutex* m;
   friend class ImageBuffer;
};
