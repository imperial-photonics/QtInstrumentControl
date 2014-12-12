#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "SerialDevice.h"

class ArduinoCounter : public SerialDevice
{
    Q_OBJECT

public:
   ArduinoCounter(QObject *parent = 0);
   ~ArduinoCounter(); 

   void GetCount();

   bool IsConnected() { return connected; }

   void SetPMTEnabled(bool enabled);

   void SetDwellTime(double dwell_time_ms_);
   double GetDwellTime() { return dwell_time_ms; }

signals:
   void CountUpdated(int count);
   void PMTEnabled(bool endabled);
   void OverloadOccured();

private:

   bool ConnectToDevice(const QString& port);
   void ResetDevice(const QString& port);
   void SendCommand(char command[], float value = NAN);

   void ReadData();

   double dwell_time_ms = 100;
   
   bool connected;
   int idx = 0;


};

#endif // MAINWINDOW_H
