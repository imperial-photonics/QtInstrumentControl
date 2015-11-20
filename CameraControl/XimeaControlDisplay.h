#pragma once

#include "XimeaCamera.h"
#include "ParameterWidget.h"

XimeaCamera* GetXimeaFromUser(bool choose_camera = true, QObject* parent = 0);


class XimeaControlDisplay : public QWidget
{
   Q_OBJECT

public:
   explicit XimeaControlDisplay(XimeaCamera* camera, QWidget* parent = 0);

private:

   /*
   Add a XimeaControlWidget

   The arguments to this function should match those to the XimeaControlWidget
   function excluding the inital XimeaCamera pointer
   */
   template<typename... Args>
   void AddWidget(Args... args)
   {
      ParameterWidget* w = new ParameterWidget(camera, args...);
      widgets.push_back(w);
      
      // Ximea api provides no callback, so manually check all widgets when one value changes
      connect(w, &ParameterWidget::ValueChanged, this, &XimeaControlDisplay::UpdateWidgets);
   }

   void UpdateWidgets()
   {
      for (auto& w : widgets)
         w->SetWidgetValue();
   }

   void UpdateStreamingStatus(bool is_streaming);

   XimeaCamera* camera;
   QList<ParameterWidget*> widgets;

};