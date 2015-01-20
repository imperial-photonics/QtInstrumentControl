#pragma once

#include "XimeaCameraPrivate.h"
#include "CameraControlWidget.h"

XimeaCameraPrivate* GetXimeaFromUser(bool choose_camera = true, QObject* parent = 0);


class XimeaControlDisplay : public QWidget
{
   Q_OBJECT

public:
   explicit XimeaControlDisplay(XimeaCameraPrivate* camera, QWidget* parent = 0);

private:

   /*
   Add a XimeaControlWidget

   The arguments to this function should match those to the XimeaControlWidget
   function excluding the inital XimeaCamera pointer
   */
   template<typename... Args>
   void AddWidget(Args... args)
   {
      CameraControlWidget* w = new CameraControlWidget(camera, args...);
      widgets.push_back(w);
      
      // Ximea api provides no callback, so manually check all widgets when one value changes
      connect(w, &CameraControlWidget::ValueChanged, this, &XimeaControlDisplay::UpdateWidgets);
   }

   void UpdateWidgets()
   {
      for (auto& w : widgets)
         w->SetWidgetValue();
   }

   void UpdateStreamingStatus(bool is_streaming);

   XimeaCameraPrivate* camera;
   QList<CameraControlWidget*> widgets;

};