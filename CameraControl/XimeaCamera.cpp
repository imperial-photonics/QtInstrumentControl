#include "XimeaCamera.h"
#include "XimeaControlDisplay.h"
#include <xiApi.h>

void Check(int v)
{
   if (v != 0)
      throw std::exception("Ximea Camera Error",v);
   }

QStringList XimeaCamera::GetConnectedCameras()
{
   QStringList list;
   char buf[1024];

   // Get number of cameras
   DWORD n_camera;
   Check(xiGetNumberDevices(&n_camera));
   for (DWORD i = 0; i < n_camera; i++)
   {
      QByteArray device;
      
      Check(xiGetDeviceInfoString(i, XI_PRM_DEVICE_NAME, buf, 1024));
      device.append(buf).append(", S/N: ");
      Check(xiGetDeviceInfoString(i, XI_PRM_DEVICE_SN, buf, 1024));
      device.append(buf);

      list.append(device);
   }

   return list;

}


XimeaCamera::XimeaCamera(int camera_idx, QObject* parent) :
AbstractStreamingCamera(parent)
{
   // Don't try and determine bandwidth
   xiSetParamInt(0, XI_PRM_AUTO_BANDWIDTH_CALCULATION, XI_OFF);

   // Retrieving a handle to the camera device 
   Check(xiOpenDevice(0, &xiH));

   StartThread();
}


void XimeaCamera::Init()
{

   // Setting "exposure" parameter (1ms)
   //Check(xiSetParam(xiH, XI_PRM_EXPOSURE, &integration_time_us, sizeof(integration_time_us), xiTypeInteger));


   // Set the buffers so that we have 100 images
   // If we get that far behind we're in trouble...
   int n_buffer = 100;
   int max_width, max_height;
   Check(xiGetParamInt(xiH, XI_PRM_WIDTH XI_PRM_INFO_MAX, &max_width));
   Check(xiGetParamInt(xiH, XI_PRM_HEIGHT XI_PRM_INFO_MAX, &max_height));

   int max_n_bytes = max_width * max_height * 4; // worst case 4 bytes

   Check(xiSetParamInt(xiH, XI_PRM_BUFFERS_QUEUE_SIZE, 10));
   Check(xiSetParamInt(xiH, XI_PRM_ACQ_BUFFER_SIZE, max_n_bytes * n_buffer));
   Check(xiSetParamInt(xiH, XI_PRM_BUFFER_POLICY, XI_BP_SAFE)); // we're going to provide our own CUDA buffers

   AllocateBuffers(max_n_bytes);

}


QWidget* XimeaCamera::GetControlWidget(QWidget* parent)
{
   return new XimeaControlDisplay(this, parent);
}

void XimeaCamera::SetTriggerMode(TriggerMode trigger_mode)
{
   if (trigger_mode == Internal)
      SetParameter(XI_PRM_TRG_SOURCE, Integer, XI_TRG_OFF);
   if (trigger_mode == Software)
      SetParameter(XI_PRM_TRG_SOURCE, Integer, XI_TRG_SOFTWARE);
   else
      SetParameter(XI_PRM_TRG_SOURCE, Integer, XI_TRG_EDGE_RISING);
}

void XimeaCamera::SoftwareTrigger()
{
   xiSetParamInt(xiH, XI_PRM_TRG_SOFTWARE, 0);
}

void XimeaCamera::SetParameter(const QString& parameter, ParameterType type, QVariant value)
{
   QByteArray b = parameter.toUtf8();
   const char* param = b.constData();

   switch (type)
   {
   case Integer:
   case Boolean:
   case Enumeration:
      Check(xiSetParamInt(xiH, param, value.toInt()));
      break;

   case Float:
      Check(xiSetParamFloat(xiH, param, value.toFloat()));
      break;

   case Text:
      QString s = value.toString();
      Check(xiSetParamString(xiH, param, s.toUtf8().data(), s.size()));
      break;

   }
}

QVariant XimeaCamera::GetParameter(const QString& parameter, ParameterType type)
{
   QByteArray b = parameter.toUtf8();
   const char* param = b.constData();

   QVariant value;

   switch (type)
   {
   case Integer:
   case Boolean:
   case Enumeration:
      int vi;
      Check(xiGetParamInt(xiH, param, &vi));
      value = QVariant::fromValue(vi);
      break;

   case Float:
      float vf;
      Check(xiGetParamFloat(xiH, param, &vf));
      value = QVariant::fromValue(vf);
      break;

   case Text:
      char vc[1024];
      Check(xiGetParamString(xiH, param, vc, 1024));
      QString s = QString::fromUtf8(vc);
      value = QVariant::fromValue(s);
      break;
   }

   return value;
}

QVariant XimeaCamera::GetParameterLimit(const QString& parameter, ParameterType type, Limit limit)
{
   QString p = parameter;
   
   if (limit == Limit::Min)
      p.append(XI_PRM_INFO_MIN);
   else
      p.append(XI_PRM_INFO_MAX);

   return GetParameter(p, type);
}

QVariant XimeaCamera::GetParameterMinIncrement(const QString& parameter, ParameterType type)
{
   QString p = parameter;

   p.append(XI_PRM_INFO_INCREMENT);
   return GetParameter(p, type);
}

EnumerationList XimeaCamera::GetEnumerationList(const QString& parameter)
{
   EnumerationList list;

   auto Add = [&](QString s, int i) { list.append(QPair<QString, int>(s, i)); };

   if (parameter == XI_PRM_TRG_SOURCE)
   {
      Add("Internal", XI_TRG_OFF);
      Add("External Rising", XI_TRG_EDGE_RISING);
      Add("External Falling", XI_TRG_EDGE_FALLING);
      Add("Software", XI_TRG_SOFTWARE);
   }
   else if (parameter == XI_PRM_ACQ_TIMING_MODE)
   {
      Add("Free Run", XI_ACQ_TIMING_MODE_FREE_RUN);
      Add("Frame Rate", XI_ACQ_TIMING_MODE_FRAME_RATE);
   }
   else if (parameter == XI_PRM_TRG_SELECTOR)
   {
      Add("Frame Start", XI_TRG_SEL_FRAME_START);
      Add("Exposure Active", XI_TRG_SEL_EXPOSURE_ACTIVE);
      Add("Frame Burst State", XI_TRG_SEL_FRAME_BURST_START);
      Add("Frame Burst Active", XI_TRG_SEL_FRAME_BURST_ACTIVE);
   }
   else if (parameter == XI_PRM_GPI_MODE)
   {
      Add("Off", XI_GPI_OFF);
      Add("Trigger", XI_GPI_TRIGGER);
      Add("External Event", XI_GPI_EXT_EVENT);
   }
   else if (parameter == XI_PRM_GPO_MODE)
   {
      Add("Off", XI_GPO_OFF);
      Add("On", XI_GPO_ON);
      Add("Frame Active", XI_GPO_FRAME_ACTIVE);
      Add("Frame Active Neg", XI_GPO_FRAME_ACTIVE_NEG);
      Add("Exposure Active", XI_GPO_EXPOSURE_ACTIVE);
      Add("Exposure Active Neg", XI_GPO_EXPOSURE_ACTIVE_NEG);
      Add("Frame Trigger Wait", XI_GPO_FRAME_TRIGGER_WAIT);
      Add("Frame Trigger Wait Neg", XI_GPO_FRAME_TRIGGER_WAIT_NEG);
      Add("Exposure Pulse", XI_GPO_EXPOSURE_PULSE);
      Add("Exposure Pulse Neg", XI_GPO_EXPOSURE_PULSE_NEG);
      Add("Busy", XI_GPO_BUSY);
      Add("Busy Neg", XI_GPO_BUSY_NEG);
   }
   else if (parameter == XI_PRM_IMAGE_DATA_FORMAT)
   {
      Add("Mono 8", XI_MONO8);
      Add("Mono 16", XI_MONO16);
      Add("Raw 8", XI_RAW8);
      Add("Raw 16", XI_RAW16);
      Add("RGB 24", XI_RGB24);
      Add("RGB Planar", XI_RGB_PLANAR);
      Add("Packed", XI_FRM_TRANSPORT_DATA);
      Add("Dummy", 100);
   }

   return list;
}


int XimeaCamera::GetNumBytesPerPixel()
{
   int bit_depth;
   Check(xiGetParamInt(xiH, XI_PRM_OUTPUT_DATA_BIT_DEPTH, &bit_depth));

   //bytes_per_pixel = ;
   return bit_depth / 8;
}

cv::Size XimeaCamera::GetImageSize()
{
   int image_width, image_height;
   Check(xiGetParamInt(xiH, XI_PRM_WIDTH, &image_width));
   Check(xiGetParamInt(xiH, XI_PRM_HEIGHT, &image_height));

   //image_size = cv::Size(image_width, image_height);
   return cv::Size(image_width, image_height);
}


int XimeaCamera::GetImageSizeBytes()
{
   return GetImageSize().area() * GetNumBytesPerPixel();
}

int XimeaCamera::GetStride()
{
   // Image stride is always width

   int image_width;
   Check(xiGetParamInt(xiH, XI_PRM_WIDTH, &image_width));
   return image_width;
}

double XimeaCamera::GetPixelSize()
{
   // Can't get this programmatically
   return 0;
}

cv::Rect XimeaCamera::GetROI()
{
   int w, h, x, y;
   Check(xiGetParamInt(xiH, XI_PRM_WIDTH, &w));
   Check(xiGetParamInt(xiH, XI_PRM_HEIGHT, &h));
   Check(xiGetParamInt(xiH, XI_PRM_OFFSET_X, &x));
   Check(xiGetParamInt(xiH, XI_PRM_OFFSET_X, &y));

   return cv::Rect(x, y, w, h);
}

void XimeaCamera::SetFullROI()
{
   int w, h, x, y;
   Check(xiGetParamInt(xiH, XI_PRM_WIDTH XI_PRM_INFO_MAX, &w));
   Check(xiGetParamInt(xiH, XI_PRM_HEIGHT XI_PRM_INFO_MAX, &h));
   Check(xiGetParamInt(xiH, XI_PRM_OFFSET_X XI_PRM_INFO_MAX, &x));
   Check(xiGetParamInt(xiH, XI_PRM_OFFSET_Y XI_PRM_INFO_MAX, &y));

   Check(xiSetParamInt(xiH, XI_PRM_WIDTH, w));
   Check(xiSetParamInt(xiH, XI_PRM_HEIGHT, h));
   Check(xiSetParamInt(xiH, XI_PRM_OFFSET_X, x));
   Check(xiSetParamInt(xiH, XI_PRM_OFFSET_Y, y));
}



void XimeaCamera::SetROI(cv::Rect roi)
{
   Check(xiSetParamInt(xiH, XI_PRM_WIDTH, roi.width));
   Check(xiSetParamInt(xiH, XI_PRM_HEIGHT, roi.height));
   Check(xiSetParamInt(xiH, XI_PRM_OFFSET_X, roi.x));
   Check(xiSetParamInt(xiH, XI_PRM_OFFSET_Y, roi.y));
}


std::shared_ptr<ImageBuffer> XimeaCamera::GrabImage()
{

   if (is_streaming)
      return GetNext();

   XI_RETURN errorValue;
   XI_IMG img;

   cv::Size image_size = GetImageSize();
   int bytes_per_pixel = GetNumBytesPerPixel();
   int image_type = CV_8U; // todo

   img.size = sizeof(XI_IMG);

   Check(xiStartAcquisition(xiH));

   img.bp_size = image_size.area() * bytes_per_pixel;
   img.bp = GetUnusedBuffer();

   errorValue = xiGetImage(xiH, 5000, &img);

   cv::Mat image(image_size, image_type, img.bp);
   SetLatest(image);
 
   Check(xiStopAcquisition(xiH));

   return GetLatest();
}

void XimeaCamera::run()
{
   XI_RETURN errorValue;
   XI_IMG img;

   cv::Size image_size = GetImageSize();
   int bytes_per_pixel = GetNumBytesPerPixel();
   int image_type = CV_8U; // todo

   img.size = sizeof(XI_IMG);

   Check(xiSetParamInt(xiH, XI_PRM_BUFFERS_QUEUE_SIZE, 10));
   Check(xiSetParamInt(xiH, XI_PRM_BUFFER_POLICY, XI_BP_SAFE)); // we're going to provide our own CUDA buffers


   Check(xiStartAcquisition(xiH));
      
//   emit ControlLockUpdated(true);
   emit StreamingStatusChanged(true);

   while (!terminate)
   {
      img.bp_size = image_size.area() * bytes_per_pixel;
      img.bp = GetUnusedBuffer();

      errorValue = xiGetImage(xiH, 5000, &img);
      
      if (terminate)
         break;

      if (errorValue == XI_OK)
      {
         cv::Mat image(image_size, image_type, img.bp);
         SetLatest(image);
      }
      /*
      else if (errorValue == 13)
      {
         CHECK(AT_Command(Hndl, L"AcquisitionStop"));
         CHECK(AT_Command(Hndl, L"AcquisitionStart"));
         cout << "Restarting Acquisition\n";
      }
      */
      else
      {
         std::cout << "Ximea error code: " << errorValue << "\n";
         break;
      }
   }

   Check(xiStopAcquisition(xiH));

   TerminateStreaming();
}

void XimeaCamera::FlushBuffers()
{
   // Do nothing
}






XimeaCamera::~XimeaCamera()
{
   if (xiH)
      xiCloseDevice(xiH);
}

void XimeaCamera::SetIntegrationTime(int integration_time_ms)
{
   integration_time_us = integration_time_ms * 1000;

   // Setting "exposure" parameter (1ms)
   Check(xiSetParam(xiH, XI_PRM_EXPOSURE, &integration_time_us, sizeof(integration_time_us), xiTypeInteger));
}


void XimeaCamera::GetImage(cv::Mat& cv_output)
{
//   Check(xiGetImage(xiH, timeout_ms, &image));
//   cv::Mat cv_image(image.height, image.width, CV_8U, image.bp);
//   return cv_image.copyTo(cv_output);
}

