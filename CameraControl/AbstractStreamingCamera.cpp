#include "AbstractStreamingCamera.h"
#include "ImageBuffer.h"
#include <QOpenGLBuffer>
#include <QTimer>

#include <cuda.h>
#include <cuda_runtime.h>

using std::vector;
using std::shared_ptr;


AbstractStreamingCamera::AbstractStreamingCamera(QObject* parent) :
   init(false), allocation_idx(0),
   is_streaming(false), terminate(false)
{
   connect(parent, &QObject::destroyed, this, &QObject::deleteLater);

   m = new QMutex();
   next_mutex = new QMutex();
   latest_data = shared_ptr<ImageBuffer>(new ImageBuffer());

   control_mutex = new QMutex(QMutex::Recursive);
};

AbstractStreamingCamera::~AbstractStreamingCamera()
{
   SetStreamingStatus(false);
   FreeBuffers();
}

void AbstractStreamingCamera::FreeBuffers()
{
   QMutexLocker lk(m);
   init = false;
   allocation_idx++;
   latest_data = shared_ptr<ImageBuffer>(new ImageBuffer());
   for( auto buffer : buffers)
      cudaFreeHost(buffer);
   buffers.clear();
};

shared_ptr<ImageBuffer> AbstractStreamingCamera::GetLatest()
{
   QMutexLocker lk(m);
   return latest_data;
};

shared_ptr<ImageBuffer> AbstractStreamingCamera::GetNext()
{
   QMutexLocker next_lk(next_mutex);
   next_cv.wait(next_mutex);

   QMutexLocker lk(m);
   return latest_data;
};

cv::Mat AbstractStreamingCamera::GetImage()
{
   // Return a copy of the image so we can safely process
   shared_ptr<ImageBuffer> buf = GetLatest();
   return buf->GetBackgroundSubtractedImage().clone();
}

cv::Mat AbstractStreamingCamera::GetNextImage()
{
   // Return a copy of the image so we can safely process
   shared_ptr<ImageBuffer> buf = GetNext();
   return buf->GetBackgroundSubtractedImage().clone();
}


void AbstractStreamingCamera::QueueAllBuffers()
{
   FlushBuffers();
   buffer_size = GetImageSizeBytes();
   for (auto buffer : buffers)
      QueuePointer(buffer);
}

void AbstractStreamingCamera::QueuePointer(unsigned char* ptr)
{
   unused_buffers.push_back(ptr);
}


void AbstractStreamingCamera::AllocateBuffers(int buffer_size_) 
{
   buffer_size = buffer_size_;
   max_buffer_size = buffer_size;

   // Don't allocate if the current buffers are good enough
   if (buffer_size < buffer_size_ || buffers.size() == 0)
   {
      QMutexLocker lk(m);
      latest_data = shared_ptr<ImageBuffer>( new ImageBuffer() );
      lk.unlock();

      FlushBuffers();
      FreeBuffers();

      // buffer_size should be big enough for float
      CHECK(cudaMallocHost(&background_ptr, buffer_size, cudaHostAllocMapped)); 

      for(int i=0; i<n_buffers; i++)
      {
         void* ptr;
         CHECK(cudaMallocHost(&ptr, buffer_size, cudaHostAllocMapped));
      
         unsigned char* u8_ptr = static_cast<unsigned char*>(ptr);
         buffers.push_back(u8_ptr);
         unused_buffers.push_back(u8_ptr);
      }
      allocation_idx++;
   }

   init = true;
}

unsigned char* AbstractStreamingCamera::GetUnusedBuffer()
{
   if (unused_buffers.empty())
      throw std::exception("Streaming Camera Error - Out of buffers");

   unsigned char* buf = unused_buffers.front();
   unused_buffers.pop_front();

   return buf;
}
   
void AbstractStreamingCamera::SetLatest(cv::Mat& image)
{
   QMutexLocker lk(m);
   latest_data = shared_ptr<ImageBuffer>( new Buf(image, background, this) );
   lk.unlock();

   emit NewImage();
   next_cv.wakeAll();
}

void AbstractStreamingCamera::SetStreamingStatus(bool streaming_)
{
   if (streaming_)
   {
      if (is_streaming)
         return;
   
      main_thread = QThread::currentThread();
      worker_thread = new QThread(this);
      worker_thread->setObjectName("Andor Camera");
      this->moveToThread(worker_thread);
 
      connect(worker_thread, &QThread::started, this, &AbstractStreamingCamera::run);
      connect(this, &AbstractStreamingCamera::StreamingFinished, [=]() { moveToThread(main_thread); });
      connect(this, &AbstractStreamingCamera::StreamingFinished, worker_thread, &QThread::quit);
      connect(worker_thread, &QThread::finished, worker_thread, &QThread::deleteLater);
      
      terminate = false;
      is_streaming = true;

      worker_thread->start();

//      QThread::sleep(1); 
   }
   else
   {
      // Try and tell the thread to quit
      terminate = true;

   }
}

void AbstractStreamingCamera::TerminateStreaming()
{
   if (is_streaming)
   {
      emit StreamingFinished();
      emit StreamingStatusChanged(false);

      QueueAllBuffers();

      is_streaming = false;
      terminate = false;
   }
}


void AbstractStreamingCamera::ClearBackground()
{
   background = cv::Mat();
   emit NewBackground();
}

void AbstractStreamingCamera::UpdateBackground()
{
   cv::Size image_size = GetImageSize();
   cv::Mat new_background(image_size, CV_32F, background_ptr);
   
   int n = 5;

   cv::Mat f;
   for (int i=0; i<n; i++)
   {
      shared_ptr<ImageBuffer> buf = GrabImage();
      buf->GetImage().convertTo(f, CV_32F);
      new_background += f;
   }

   new_background /= n;
   new_background.convertTo(background, CV_16U);

   emit NewBackground();
}

cv::Mat AbstractStreamingCamera::BackgroundImage()
{
   return background;
}