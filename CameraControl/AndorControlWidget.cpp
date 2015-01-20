#include "AndorControlWidget.h"

#include <QCoreApplication>
#include <QSpinBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QTimer>
#include <QPushButton>
#include <QMutexLocker>

#include <iostream>

AndorControlWidget::AndorControlWidget(AndorCamera* camera, QFormLayout* parent, QString control, ControlType type, QString suffix, bool use_timer) :
   CameraControlWidget(camera, parent, control, type, suffix, use_timer, false), 
   Hndl(camera->GetHandle())
{
   const AT_WC* param = reinterpret_cast<const AT_WC*>(control.utf16());

   AT_IsImplemented(Hndl, param, &is_implemented);
   if (!is_implemented)
      return;

   Init();

   CHECK(AT_RegisterFeatureCallback(Hndl, param, ValueUpdatedCallback, static_cast<void*>(this)));

};

AndorControlWidget::~AndorControlWidget()
{
   const AT_WC* param = control.toStdWString().data();

   if (is_implemented)
      AT_UnregisterFeatureCallback(Hndl, param, ValueUpdatedCallback, static_cast<void*>(this));
}

int AT_EXP_CONV ValueUpdatedCallback(AT_H Hndl, const AT_WC* Feature, void* context)
{   
   AndorControlWidget* widget = static_cast<AndorControlWidget*>(context);
   
   // This may be called from another thread so post as event
   QCoreApplication::postEvent(widget, new QEvent(WidgetEvent::ValueUpdated));
   return 0;
}
