
#define NOMINMAX

#include "AndorCamera.h"
#include "AndorControlDisplay.h"

#include <QOpenGLBuffer>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QStringList>
#include <QVector>
#include <QSettings>
#include <QDir>

#include <map>
#include <iostream>

using namespace std;

namespace AndorCameras
{
   extern int n_andor_cameras_connected__ = 0;
   extern bool andor_lib_init__ = false;
}




AndorCamera::AndorCamera(int idx, QObject* parent) :
   AbstractStreamingCamera(parent), Hndl(0)
{
   using namespace AndorCameras;

   if (!andor_lib_init__)
   {
      CHECK(AT_InitialiseLibrary()); 
      andor_lib_init__ = true;
   }

   // Open camera
   CHECK(AT_Open(idx, &Hndl));
   n_andor_cameras_connected__++;

   // Allocate buffers big enough for largest possible image
   //===========================================================
   int64_t max_width, max_height;
   double max_bytes_per_pixel;
   AT_GetIntMax(Hndl,L"AOIWidth", &max_width);
   AT_GetIntMax(Hndl,L"AOIWidth", &max_height);
   CHECK(AT_GetFloatMax(Hndl, L"BytesPerPixel", &max_bytes_per_pixel));

   int max_n_bytes = max_width * max_height * max_bytes_per_pixel;

   AllocateBuffers(max_n_bytes);
   
   // Load defaults from file
   //===========================================================
   QSettings settings("AndorDefaults.ini",  QSettings::IniFormat);

   int64_t value;
   value = settings.value("AOIWidth/Value", 2048).toInt();
   SOFTCHECK(AT_SetInt(Hndl, L"AOIWidth", value));
   value = settings.value("AOILeft/Value", 1).toInt();
   SOFTCHECK(AT_SetInt(Hndl, L"AOILeft", value));
   value = settings.value("AOIHeight/Value", 2048).toInt();
   SOFTCHECK(AT_SetInt(Hndl, L"AOIHeight", value));
   value = settings.value("AOITop/Value", 1).toInt();
   SOFTCHECK(AT_SetInt(Hndl, L"AOITop", value));

   double exposure = settings.value("ExposureTime/Value", 0.01).toFloat();
   SOFTCHECK(AT_SetFloat(Hndl, L"ExposureTime", exposure));

   QStringList groups = settings.childGroups();

   for(QString group : groups)
   {
      wchar_t buf[1024];
      group.toWCharArray(buf);
      buf[ group.length() ] = 0;

      settings.beginGroup(group);
      QString type = settings.value("Type","").toString();

      if (type == "Integer")
	   {
		   int64_t value = settings.value("Value").toInt();
         SOFTCHECK(AT_SetInt(Hndl, buf, value));
	   } 
      else if (type == "Float")
      {
		   double value = settings.value("Value").toDouble();
         SOFTCHECK(AT_SetFloat(Hndl, buf, value));
      }
      else if (type == "Boolean")
      {
		   bool value = settings.value("Value").toBool();
         SOFTCHECK(AT_SetBool(Hndl, buf, value));
      }
      else if (type == "Enumeration")
      {
		   QString value = settings.value("Value").toString();

         wchar_t val_buf[1024];
         value.toWCharArray(val_buf);
         val_buf[ value.length() ] = 0;

         SOFTCHECK(AT_SetEnumeratedString(Hndl, buf, val_buf));
      }

      settings.endGroup();

   }

   /*
   // Turn on cooling
   SOFTCHECK(AT_SetBool(Hndl, L"SensorCooling", AT_TRUE));
   
   // Initial configuration
   SOFTCHECK(AT_SetFloat(Hndl, L"ExposureTime", 0.1));
   SOFTCHECK(AT_SetInt(Hndl, L"AOIHeight", 2048));
   SOFTCHECK(AT_SetInt(Hndl, L"AOIWidth", 2048));
   */

   // Register callbacks for image size change
   CHECK(AT_RegisterFeatureCallback(Hndl, L"AOIHeight",     AndorFeatureCallback, static_cast<void*>(this)));
   CHECK(AT_RegisterFeatureCallback(Hndl, L"AOIWidth",      AndorFeatureCallback, static_cast<void*>(this)));
   CHECK(AT_RegisterFeatureCallback(Hndl, L"BytesPerPixel", AndorFeatureCallback, static_cast<void*>(this)));
}


QWidget* AndorCamera::GetControlWidget(QWidget* parent)
{
   return new AndorControlDisplay(this, parent);
}



AndorCamera::~AndorCamera()
{
   FreeBuffers();
   
   using namespace AndorCameras;

   AT_Close(Hndl);
   n_andor_cameras_connected__--;

   if (n_andor_cameras_connected__ == 0)
   {
      AT_FinaliseLibrary();
      andor_lib_init__ = false;
   }
}

int AT_EXP_CONV AndorFeatureCallback(AT_H Hndl, const AT_WC* feature, void* context)
{
   AndorCamera* camera = static_cast<AndorCamera*>(context);
   camera->Callback(feature);
   return true;
}

void AndorCamera::Callback(const AT_WC* feature)
{
   GetImageSize();
   GetNumBytesPerPixel();

   // requeue buffers with correct size
   QueueAllBuffers();

   // Only size updates registered
   emit ImageSizeChanged();
}


void AndorCamera::SetParameter(const QString& parameter, ParameterType type, QVariant value)
{
   const AT_WC* param = reinterpret_cast<const AT_WC*>(parameter.utf16());

   switch (type)
   {
   case Integer:
      SOFTCHECK(AT_SetInt(Hndl, param, value.toLongLong()));
      break;

   case Float:
      SOFTCHECK(AT_SetFloat(Hndl, param, value.toDouble()));
      break;

   case Boolean:
      SOFTCHECK(AT_SetBool(Hndl, param, value.toInt()));
      break;

   case Text:
      SOFTCHECK(AT_SetString(Hndl, param, value.toString().toStdWString().data()));
      break;

   case Enumeration:
      SOFTCHECK(AT_SetEnumIndex(Hndl, param, value.toInt()));
   }
}

QVariant AndorCamera::GetParameter(const QString& parameter, ParameterType type)
{
   const AT_WC* param = reinterpret_cast<const AT_WC*>(parameter.utf16());
   QVariant value;

   switch (type)
   {
   case Integer:
      int64_t vl;
      SOFTCHECK(AT_GetInt(Hndl, param, &vl));
      value = QVariant::fromValue(vl);
      break;

   case Float:
      double vd;
      SOFTCHECK(AT_GetFloat(Hndl, param, &vd));
      value = QVariant::fromValue(vd);
      break;

   case Boolean:
      int vi;
      SOFTCHECK(AT_GetBool(Hndl, param, &vi));
      value = QVariant::fromValue(vi);
      break;

   case Text:
      AT_WC vc[1024];
      SOFTCHECK(AT_GetString(Hndl, param, vc, 1024));
      value = QVariant::fromValue(QString::fromWCharArray(vc));
      break;

   case Enumeration:
      SOFTCHECK(AT_GetEnumIndex(Hndl, param, &vi));
      value = QVariant::fromValue(vi);
   }

   return value;
}

QVariant AndorCamera::GetParameterMinIncrement(const QString& parameter, ParameterType type)
{
   if (type == Integer)
      return 1;

   return QVariant();
}


QVariant AndorCamera::GetParameterLimit(const QString& parameter, ParameterType type, Limit limit)
{
   const AT_WC* param = reinterpret_cast<const AT_WC*>(parameter.utf16());
   QVariant value;

   if (limit == Limit::Min)
   {
      switch (type)
      {
      case Integer:
         int64_t vl;
         SOFTCHECK(AT_GetIntMin(Hndl, param, &vl));
         value = QVariant::fromValue(vl);
         break;

      case Float:
         double vd;
         SOFTCHECK(AT_GetFloatMin(Hndl, param, &vd));
         value = QVariant::fromValue(vd);
         break;
      }
   }
   else
   {
      switch (type)
      {
      case Integer:
         int64_t vl;
         SOFTCHECK(AT_GetIntMax(Hndl, param, &vl));
         value = QVariant::fromValue(vl);
         break;

      case Float:
         double vd;
         SOFTCHECK(AT_GetFloatMax(Hndl, param, &vd));
         value = QVariant::fromValue(vd);
         break;
      }
   }

   return value;
}

EnumerationList AndorCamera::GetEnumerationList(const QString& parameter)
{
   const AT_WC* param = reinterpret_cast<const AT_WC*>(parameter.utf16());

   int count = 0;
   AT_WC buf[1024];

   SOFTCHECK(AT_GetEnumCount(Hndl, param, &count));

   EnumerationList list;

   for (int i = 0; i < count; i++)
   {
      int is_implemented = 0;
      int is_available = 0;

      SOFTCHECK(AT_IsEnumIndexImplemented(Hndl, param, i, &is_implemented));
      SOFTCHECK(AT_IsEnumIndexAvailable(Hndl, param, i, &is_available));
      SOFTCHECK(AT_GetEnumeratedString(Hndl, param, i, buf, 1024));

      if ((is_implemented && is_available))
         list.append(QPair<QString, int>(QString::fromWCharArray(buf), i));
   }

   return list;
}

bool AndorCamera::IsParameterWritable(const QString& parameter)
{
   const AT_WC* param = reinterpret_cast<const AT_WC*>(parameter.utf16());

   int is_writable = AT_FALSE;
   SOFTCHECK(AT_IsWritable(Hndl, param, &is_writable));
   return is_writable;
}

bool AndorCamera::IsParameterReadOnly(const QString& parameter)
{
   const AT_WC* param = reinterpret_cast<const AT_WC*>(parameter.utf16());

   int is_readonly = AT_TRUE;
   SOFTCHECK(AT_IsReadOnly(Hndl, param, &is_readonly));
   return is_readonly;
}

void AndorCamera::GetConnectedCameras(QStringList& camera_list, QVector<int>& camera_idx)
{
   using namespace AndorCameras;

   if (!andor_lib_init__)
   {
      CHECK(AT_InitialiseLibrary()); 
      andor_lib_init__ = true;
   }

   
   int64_t n_cameras = 0;

   camera_list.clear();
   camera_idx.clear();

   CHECK(AT_GetInt(AT_HANDLE_SYSTEM, L"DeviceCount", &n_cameras)); 

   QStringList cameras;

   for(int i=0; i<n_cameras; i++)
   {
      AT_H h;
      if ( AT_Open(i, &h) == AT_SUCCESS )
      {
         AT_WC serial_no[1024] = L""; 
         AT_WC model[1024] = L""; 

         CHECK(AT_GetString(h, L"SerialNumber", serial_no, 1024)); 
         CHECK(AT_GetString(h, L"CameraModel", model, 1024)); 
      
         QString cam_str = QString::fromWCharArray(model);
         cam_str.append(" : ");
         cam_str.append(QString::fromWCharArray(serial_no));

         camera_list.push_back( cam_str );
         camera_idx.push_back( i );

         CHECK(AT_Close(h));
      }
   }

}



AT_H AndorCamera::GetHandle()
{ 
   return Hndl; 
};


int AndorCamera::GetImageSizeBytes()
{
   int64_t image_size_bytes;
   CHECK(AT_GetInt(Hndl, L"ImageSizeBytes", &image_size_bytes));
   return (int) image_size_bytes;
}


int AndorCamera::GetNumBytesPerPixel()
{
   double bytes_per_pixel_;
   CHECK(AT_GetFloat(Hndl, L"BytesPerPixel", &bytes_per_pixel_));
   //this->bytes_per_pixel = bytes_per_pixel_;
   return bytes_per_pixel_;
}

cv::Size AndorCamera::GetImageSize()
{
   int64_t image_width;
   int64_t image_height;
   int64_t stride_;
   CHECK(AT_GetInt(Hndl, L"AOIWidth", &image_width));
   CHECK(AT_GetInt(Hndl, L"AOIHeight", &image_height));
   CHECK(AT_GetInt(Hndl, L"AOIStride", &stride_));
   
   //this->image_size = cv::Size(image_width, image_height);
   //this->stride = stride_;

   return cv::Size(image_width, image_height);
}

int AndorCamera::GetStride()
{
   int64_t stride;
   CHECK(AT_GetInt(Hndl, L"AOIStride", &stride));
   return stride;
}

double AndorCamera::GetPixelSize()
{
   double px_size;
   CHECK(AT_GetFloat(Hndl, L"PixelHeight", &px_size));
   return px_size;
}

void AndorCamera::SetROI(cv::Rect roi)
{
   int64_t p;

   if (controls_locked)
      return;

   // Try and constrain to reasonable numbers

   CHECK(AT_GetIntMax(Hndl, L"AOIWidth", &p));
   roi.width = min((int)p, roi.width);
   SOFTCHECK(AT_SetInt(Hndl, L"AOIWidth", roi.width));
      
   CHECK(AT_GetIntMin(Hndl, L"AOILeft", &p));
   roi.x = max((int)p, roi.x + 1); // Andor is 1 indexed
   SOFTCHECK(AT_SetInt(Hndl, L"AOILeft", roi.x));
      
   CHECK(AT_GetIntMax(Hndl, L"AOIHeight", &p));
   roi.height = min((int)p, roi.height);
   SOFTCHECK(AT_SetInt(Hndl, L"AOIHeight", roi.height));

   CHECK(AT_GetIntMin(Hndl, L"AOITop", &p));
   roi.y = max((int)p, roi.y + 1); // Andor is 1 indexed
   SOFTCHECK(AT_SetInt(Hndl, L"AOITop", roi.y));
}

void AndorCamera::SetUseExternalTriggering(bool use_external_triggering)
{
   if (use_external_triggering)
      AT_SetEnumeratedString(Hndl, L"TriggerMode", L"External");
   else
      AT_SetEnumeratedString(Hndl, L"TriggerMode", L"External");
}

void AndorCamera::SetFullROI()
{
   int64_t p;

   CHECK(AT_GetIntMax(Hndl, L"AOIWidth", &p));
   SOFTCHECK(AT_SetInt(Hndl, L"AOIWidth", p));

   CHECK(AT_GetIntMin(Hndl, L"AOILeft", &p));
   SOFTCHECK(AT_SetInt(Hndl, L"AOILeft", p));

   CHECK(AT_GetIntMax(Hndl, L"AOIHeight", &p));
   SOFTCHECK(AT_SetInt(Hndl, L"AOIHeight", p));

   CHECK(AT_GetIntMin(Hndl, L"AOITop", &p));
   SOFTCHECK(AT_SetInt(Hndl, L"AOITop", p));

}

cv::Rect AndorCamera::GetROI()
{
   cv::Rect roi;
   int64_t p;

   CHECK(AT_GetInt(Hndl, L"AOILeft", &p));
   roi.x = (int) p;

   CHECK(AT_GetInt(Hndl, L"AOITop", &p));
   roi.y = (int) p;
   
   CHECK(AT_GetInt(Hndl, L"AOIWidth", &p));
   roi.width = (int) p;
   
   CHECK(AT_GetInt(Hndl, L"AOIHeight", &p));
   roi.height = (int) p;

   return roi;
}


void AndorCamera::QueuePointerWithCamera(AT_U8* ptr)
{
   SOFTCHECK(AT_QueueBuffer(Hndl, ptr, buffer_size)); 
}



void AndorCamera::run()
{
   int errorValue;

   cv::Size size = GetImageSize();
   int num_bytes = GetNumBytesPerPixel();
   int type;
   if (num_bytes == 1)
      type = CV_8U;
   else if (num_bytes == 2)
      type = CV_16U;
   int stride = GetStride();

   // Start Acquisition
   //========================================
   CHECK(AT_SetEnumString(Hndl, L"CycleMode", L"Continuous"));
   CHECK(AT_Command(Hndl, L"AcquisitionStart"));

   emit ControlLockUpdated(true);
   emit StreamingStatusChanged(true);

   while(!terminate)
	{
      int ret_buffer_size;
      AT_U8* ptr;
      

      errorValue = AT_WaitBuffer(Hndl, &ptr, &ret_buffer_size, 1000);
      //printf("New image in pointer %#010x\n", ptr);

      if (terminate)
         break;

      if (errorValue == AT_SUCCESS)
      {
         SetLatest(cv::Mat(size, type, ptr, stride));
      } 
      else if (errorValue == 13)
      {
         CHECK(AT_Command(Hndl, L"AcquisitionStop"));
         CHECK(AT_Command(Hndl, L"AcquisitionStart"));
         cout << "Restarting Acquisition\n";
      }
      else 
      {
         cout << "Andor error code: " << errorValue << "\n";
         break;
      }
   }
	
   CHECK(AT_Command(Hndl, L"AcquisitionStop"));
   
   emit ControlLockUpdated(false);
   controls_locked = false;

   TerminateStreaming();
}


void AndorCamera::FlushBuffers()
{
   CHECK(AT_Flush(Hndl));
}




shared_ptr<Buf> AndorCamera::GrabImage()
{
   int errorValue;

   if (is_streaming)
      return GetNext();

   cv::Size size = GetImageSize();
   int num_bytes = GetNumBytesPerPixel();
   int type;
   if (num_bytes == 1)
      type = CV_8U;
   else if (num_bytes == 2)
      type = CV_16U;
   int stride = GetStride();

   CHECK(AT_Command(Hndl, L"AcquisitionStart"));
    
   emit ControlLockUpdated(true);

   int ret_buffer_size;
   AT_U8* ptr;
      
   errorValue = AT_WaitBuffer(Hndl, &ptr, &ret_buffer_size, AT_INFINITE);

   if (errorValue != AT_SUCCESS)
   {
      printf("Andor error code: %d\n", errorValue);
   }
   else
   {
      SetLatest(cv::Mat(size, type, ptr, stride));
   }
   CHECK(AT_Command(Hndl, L"AcquisitionStop")); 


   emit ControlLockUpdated(false);

   return GetLatest();

}



