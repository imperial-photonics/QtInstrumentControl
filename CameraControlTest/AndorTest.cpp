#include "AndorControlDisplay.h"
#include "XimeaControlDisplay.h"
#include "ImageRenderWindow.h"
#include <QApplication>
#include <QInputDialog>
#include <QErrorMessage>
#include <QVBoxLayout>
#include <QPushButton>
#include <iostream>


#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

void CHECK(int err)
{
   if (err != AT_SUCCESS)
   {
      cout << "Error: " << err << "\n";
      throw;
   }
}

void SOFTCHECK(int err)
{
   if (err != AT_SUCCESS)
   {
      cout << "Warning: " << err << "\n";
   }
}


int main(int argc, char *argv[])
{
   thread camera_thread;
   XimeaCamera* camera;

   bool setup = false;
   mutex m;
   condition_variable cv;


   QApplication qapp(argc, argv);
   camera = GetXimeaFromUser();

   XimeaControlDisplay display(camera);
   
   ImageRenderWindow* render_win = new ImageRenderWindow("Camera", camera);

   QWidget* win = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout;
   QPushButton* button = new QPushButton;

   button->setText("Streaming");
   button->setCheckable(true);

   QObject::connect(button, &QPushButton::toggled, camera, &AbstractStreamingCamera::SetStreamingStatus, Qt::DirectConnection);
   QObject::connect(camera, &AbstractStreamingCamera::StreamingStatusChanged, button, &QPushButton::setChecked, Qt::QueuedConnection);

   layout->addWidget(&display);
   layout->addWidget(button);

   win->setLayout(layout);
   win->show();

   render_win->show();


   qapp.exec();
}


