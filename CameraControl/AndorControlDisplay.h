#pragma once

#include "AndorCamera.h"
#include "AndorControlWidget.h"

AndorCamera* GetCameraFromUser(bool choose_camera = true, QObject* parent = 0);


class AndorControlDisplay : public QWidget
{
   Q_OBJECT

public: 
   explicit AndorControlDisplay(AndorCamera* camera, QWidget* parent = 0);
   
private:

   /*
      Add a AndorControlWidget

      The arguments to this function should match those to the AndorControlWidget 
      function excluding the inital AndorCamera pointer
   */
   template<typename... Args>
   void AddWidget(Args... args)
   {
      widgets.push_back(new AndorControlWidget(camera, args...));
   }

   void UpdateStreamingStatus(bool is_streaming);

   AndorCamera* camera;
   QList<AndorControlWidget*> widgets;
 
};