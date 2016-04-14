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

   virtual cv::Mat getImage() = 0;

   // override to return an unsafe reference to the current image
   // any calling function should use it quickly!
   virtual cv::Mat getImageUnsafe() = 0;

   virtual cv::Mat getNextImage() { return getImage(); };

   virtual void setImageProductionStatus(bool producing_images_) { producing_images = producing_images_; };
   virtual bool getImageProductionStatus() { return producing_images; }

signals:
   void newImage();

protected:
   bool producing_images = true;
};
