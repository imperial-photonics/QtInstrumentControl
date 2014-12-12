#pragma once

#include <QObject>
#include <Qthread>
#include "ThreadedObject.h"


class ThorlabsAPTController : public ThreadedObject
{
   Q_OBJECT
public:

   ThorlabsAPTController(QObject* parent = 0) :
      ThreadedObject(parent)
   {

   }

   void GetDevices();

   void Init();
};