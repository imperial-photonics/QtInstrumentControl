#include <QApplication>

#include "ArduinoCounterTest.h"


void main()
{
   qRegisterMetaType<QSerialPort::SerialPortError>("QSerialPort::SerialPortError");

   int argc = 0;
   char** argv = 0;

   QApplication qapp(argc, argv);


   ArduinoCounterTest test;
   test.show();


   qapp.exec();
}