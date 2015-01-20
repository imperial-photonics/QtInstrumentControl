#pragma once

#include "CameraControlWidget.h"
#include "AndorCamera.h"

class AndorControlWidget : public CameraControlWidget
{
   Q_OBJECT

public:
   AndorControlWidget(AndorCamera* camera, QFormLayout* parent, QString control, ControlType type, QString suffix = "", bool use_timer = false);
   ~AndorControlWidget();
   
protected:

   bool IsWritable();
   bool IsReadOnly();

private:

   AT_H Hndl;
   int is_implemented;

   friend class AndorControlDisplay;
   friend int AT_EXP_CONV ValueUpdatedCallback(AT_H Hndl, const AT_WC* Feature, void* context);

};
