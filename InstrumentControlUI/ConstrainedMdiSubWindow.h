#pragma once

#include <QMdiSubWindow>
#include <QVBoxLayout>

class ConstrainedMdiSubWindow : public QMdiSubWindow
{
   Q_OBJECT

public:
   ConstrainedMdiSubWindow(QWidget * parent = 0, Qt::WindowFlags windowFlags = 0) : 
      QMdiSubWindow(parent, windowFlags)
   {
      setAttribute(Qt::WA_DeleteOnClose);
   }

protected:
   
   void resizeEvent(QResizeEvent* event)
   {
      auto w = widget();
      auto s = size();

      bool resize_required = false;

      if (w != nullptr)
      {
         if (w->maximumWidth() < s.width())
         {
            s.setWidth(w->maximumWidth());
            resize_required = true;
            resize(s);
         }

         if (w->maximumHeight() < s.height())
         {
            s.setHeight(w->maximumHeight());
            resize_required = true;
         }
      }

      if (resize_required)
         resize(s);
      else
         QMdiSubWindow::resizeEvent(event);


   }
   
};