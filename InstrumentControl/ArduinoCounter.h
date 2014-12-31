#pragma once

#include "SerialDevice.h"
#include "cv.h"

// Admin commands
#define MSG_IDENTIFY 0x49 // 'I'
#define MSG_IDENTITY 0x4A

// Setup commands
#define MSG_SET_MODE 0x01
#define MSG_SET_PIXEL_CLOCK_SOURCE 0x02
#define MSG_SET_LAG_TIME 0x03
#define MSG_SET_DWELL_TIME 0x04
#define MSG_SET_NUM_PIXEL 0x05
#define MSG_SET_EXTERNAL_CLOCK_DIVISOR 0x06
#define MSG_SET_TRIGGER_CLOCK_DIVISOR 0x07

// Triggering commands
#define MSG_TRIGGER 0x10
#define MSG_START_LINE 0x11
#define MSG_START_FRAME 0x12

#define MSG_PIXEL_DATA 0x20
#define MSG_LINE_DATA 0x21

#define MODE_STREAMING  0x01
#define MODE_ON_DEMAND  0x02

#define PIXEL_CLOCK_INTERNAL 0x01
#define PIXEL_CLOCK_EXTERNAL 0x02




class ArduinoCounter : public SerialDevice
{
    Q_OBJECT

public:
   ArduinoCounter(QObject *parent = 0);
   ~ArduinoCounter(); 

   void Init();

   void GetCount();

   bool IsConnected() { return connected; }

   void SetPMTEnabled(bool enabled);

   void SetStreaming(bool streaming);
   void SetUseExternalPixelClock(bool use_external);
   void SetTriggerDivisor(int trigger_divisor);
   void SetExternalClockDivisor(int ext_clock_divisor);
   void SetDwellTime(double dwell_time_ms_);
   void SetPixelsPerLine(int pixels_per_line_);

   bool GetStreaming() { return streaming; }
   bool GetUseExternalClock() { return use_external_clock; }
   int GetTriggerDivisor() { return trigger_divisor; };
   int GetExternalClockDivisor() { return ext_clock_divisor; }
   double GetDwellTime() { return dwell_time_ms; }
   int GetPixelsPerLine() { return pixels_per_line; }

   void StartLine();
   void StartFrame();

signals:
   void CountUpdated(int count);
   void NewLine(cv::Mat line);
   void PMTEnabled(bool endabled);
   void OverloadOccured();

private:

   bool ConnectToDevice(const QString& port);
   void ResetDevice(const QString& port);
   void SendCommand(char command[], float value = NAN);

   template <typename T>
   void SendMessage(char msg, T param, bool require_connection = true);
   void SendMessage(char msg) { SendMessage(msg, uint32_t(0)); }

   QByteArray WaitForMessage(char msg, int timeout_ms = 1000);
   QByteArray ProcessMessage(QByteArray msg);
   QByteArray current_message;
   qint64 bytes_left_in_message = 5;
   void MonitorCount();

   QByteArray ReadBytes(int n_bytes, int timeout_ms = 10000);
   void ReadData();

   bool streaming = false;
   double dwell_time_ms = 100;
   bool use_external_clock = false;
   int ext_clock_divisor = 1;
   int trigger_divisor = 4;
   int pixels_per_line = 1;

   //bool connected;
   int idx = 0;

   QTimer* monitor_timer;

};


template <typename T>
void ArduinoCounter::SendMessage(char msg, T param, bool require_connection)
{
   static_assert(sizeof(param) == 4, "Parameter size must be 4 bytes");

   if (require_connection && !connected)
      return;

   QMutexLocker lk(&connection_mutex);

   char* p = reinterpret_cast<char*>(&param);

   bool a = serial_port->isOpen();

   serial_port->write(&msg, 1);
   serial_port->write(p, 4);
   serial_port->flush();
}

