#include "AbstractStreamingCamera.h"
#include "ImageBuffer.h"
#include <QOpenGLBuffer>
#include <QTimer>

#ifdef USE_CUDA
#include <cuda.h>
#include <cuda_runtime.h>
#endif

using std::vector;
using std::shared_ptr;


AbstractStreamingCamera::AbstractStreamingCamera(QObject* parent) :
   is_init(false), 
   allocation_idx(0),
   terminate(false)
{
   if (parent != nullptr)
      connect(parent, &QObject::destroyed, this, &QObject::deleteLater);

   m = new QMutex();
   buffer_mutex = new QMutex(QMutex::Recursive);
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
   QMutexLocker lkb(buffer_mutex);
   is_init = false;
   allocation_idx++;
   latest_data = shared_ptr<ImageBuffer>(new ImageBuffer());
   for( auto buffer : buffers)
      FreeMemory(buffer);
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
   next_cv.wait(next_mutex, 10000);

   QMutexLocker lk(m);
   return latest_data;
};

cv::Mat AbstractStreamingCamera::GetImage()
{
   // Return a copy of the image so we can safely process
   shared_ptr<ImageBuffer> buf = GetLatest();
   return buf->GetBackgroundSubtractedImage().clone();
}


cv::Mat AbstractStreamingCamera::GetImageUnsafe()
{
   // Return a copy of the image so we can safely process
   shared_ptr<ImageBuffer> buf = GetLatest();
   return buf->GetImage();
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

/*
   Call to return a used buffer to the queue.
   Generally called from ImageBuffer
*/
void AbstractStreamingCamera::QueuePointer(unsigned char* ptr)
{
   QueuePointerWithCamera(ptr);
   
   QMutexLocker lk(buffer_mutex);
   unused_buffers.push_back(ptr);
}

/*
   Wrapper for memory allocation, use cuda pinned memory if possible,
   otherwise just malloc
*/
void AbstractStreamingCamera::AllocateMemory(void** ptr, int size)
{
#ifdef USE_CUDA
   CHECK(cudaMallocHost(ptr, size, cudaHostAllocMapped)); 
#else
   *ptr = malloc(size);
#endif
}

/*
   Wrapper for memory free
*/
void AbstractStreamingCamera::FreeMemory(void* ptr)
{
#ifdef USE_CUDA
   CHECK(cudaFreeHost(ptr));
#else
   free(ptr);
#endif
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

      QMutexLocker lkb(buffer_mutex);

      FlushBuffers();
      FreeBuffers();


      // buffer_size should be big enough for float
      AllocateMemory(reinterpret_cast<void**>(&background_ptr), buffer_size);
      
      for(int i=0; i<n_buffers; i++)
      {
         void* ptr;
         AllocateMemory(&ptr, buffer_size);

         unsigned char* u8_ptr = static_cast<unsigned char*>(ptr);
         buffers.push_back(u8_ptr);
         unused_buffers.push_back(u8_ptr);
      }
      allocation_idx++;
   }

   is_init = true;
}

/*
   Call this function to get a new buffer to store an image during streaming
*/
unsigned char* AbstractStreamingCamera::GetUnusedBuffer()
{
   QMutexLocker lk(buffer_mutex);

   if (unused_buffers.empty())
      throw std::exception("Streaming Camera Error - Out of buffers");

   unsigned char* buf = unused_buffers.front();
   unused_buffers.pop_front();

   return buf;
}
   
/*
   Call this function from the streaming thread with new image data
*/
void AbstractStreamingCamera::SetLatest(cv::Mat& image)
{
   QMutexLocker lk(m);
   latest_data = shared_ptr<ImageBuffer>( new ImageBuffer(image, background, this, image_index++) );
   lk.unlock();

   emit NewImage();
   next_cv.wakeAll();
}

/*
   Turn streaming on or off
   Call this function directly from the main thread
*/
void AbstractStreamingCamera::SetStreamingStatus(bool streaming_)
{
   if (streaming_)
   {
      if (is_streaming)
         return;
   
      //main_thread = QThread::currentThread();
      worker_thread = new QThread;
      worker_thread->setObjectName("Andor Camera");
      
      connect(worker_thread, &QThread::started, this, &AbstractStreamingCamera::run);
      //connect(this, &AbstractStreamingCamera::StreamingFinished, [=]() { moveToThread(main_thread); });
      connect(this, &AbstractStreamingCamera::StreamingFinished, worker_thread, &QThread::quit);
      connect(worker_thread, &QThread::finished, worker_thread, &QThread::deleteLater);
      
      terminate = false;
      is_streaming = true;
      image_index = 0;

      worker_thread->start();
   }
   else
   {
      // Try and tell the thread to quit and wait until it does
      terminate = true;
      //while (is_streaming) {};
   }
}

/*
   Call this function just before the streaming thread finishes
   to clean up the buffers and notify listeners
*/
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

/*
   Discard the stored background
*/
void AbstractStreamingCamera::ClearBackground()
{
   background = cv::Mat();
   emit NewBackground();
}

/*
   Collect a new background by averaging a number of frames
*/
void AbstractStreamingCamera::UpdateBackground()
{
   cv::Size image_size = GetImageSize();
   cv::Mat new_background(image_size, CV_32F, background_ptr);
   /*
   
   int n = 5;

   int type;

   cv::Mat f;
   for (int i=0; i<n; i++)
   {
      shared_ptr<ImageBuffer> buf = GrabImage();
      buf->GetImage().convertTo(f, CV_32F);
      new_background += f;

      type = buf->GetImage().type();
   }

   new_background /= n;
   new_background.convertTo(background, type);
   */

   shared_ptr<ImageBuffer> buf = GrabImage();
   buf->GetImage().copyTo(background);

   emit NewBackground();
}

cv::Mat AbstractStreamingCamera::BackgroundImage()
{
   return background;
}