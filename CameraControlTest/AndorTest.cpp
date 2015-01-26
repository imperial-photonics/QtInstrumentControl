#include "AndorControlDisplay.h"
#include "XimeaControlDisplay.h"

#include <QApplication>
#include <QInputDialog>
#include <QErrorMessage>

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
   XimeaCameraPrivate* camera;

   bool setup = false;
   mutex m;
   condition_variable cv;


   QApplication qapp(argc, argv);
   camera = GetXimeaFromUser();

   XimeaControlDisplay display(camera);
   display.show();

   qapp.exec();
}


