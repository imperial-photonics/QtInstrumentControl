#pragma once

#include "ThreadedObject.h"
#include <cv.h>

/*
Generic interface describing an object which generates images
*/
class ImageSource : public ThreadedObject
{
   Q_OBJECT

public:

   ImageSource(QObject* parent = 0, QThread* thread = 0) :
      ThreadedObject(parent, thread)
   {}

   virtual cv::Mat GetImage() = 0;

   // override to return an unsafe reference to the current image
   // any calling function should use it quickly!
   virtual cv::Mat GetImageUnsafe() = 0;

   virtual cv::Mat GetNextImage() { return GetImage(); };

   virtual void SetImageProductionStatus(bool producing_images_) { producing_images = producing_images_; };
   virtual bool GetImageProductionStatus() { return producing_images; }

signals:
   void NewImage();

protected:
   bool producing_images = true;
};
