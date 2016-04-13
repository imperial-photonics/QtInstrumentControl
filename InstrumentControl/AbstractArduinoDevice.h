#pragma once

#pragma once

#include "SerialDevice.h"
#include <opencv2/core.hpp>

// Admin commands
#define MSG_IDENTIFY 0x49 // 'I'
#define MSG_IDENTITY 0x4A


// Setup commands
#define MSG_SET_MODE 0x01
#define MSG_SET_PIXEL_CLOCK_SOURCE 0x02
#define MSG_SET_LINE_CLOCK_SOURCE 0x03
#define MSG_SET_LAG_TIME 0x04
#define MSG_SET_DWELL_TIME 0x05
#define MSG_SET_NUM_PIXEL 0x06
#define MSG_SET_EXTERNAL_CLOCK_DIVISOR 0x07
#define MSG_SET_TRIGGER_CLOCK_DIVISOR 0x08
#define MSG_SET_GALVO_OFFSET 0x09
#define MSG_SET_GALVO_STEP 0x0A
#define MSG_SET_NUM_FLYBACK_STEPS 0x0B
#define MSG_USE_DIAGNOSTIC_COUNTS 0x0C
#define MSG_SET_TRIGGER_DELAY 0x0D
#define MSG_SET_TRIGGER_DURATION 0x0E

// Triggering commands
#define MSG_TRIGGER 0x10
#define MSG_START_LINE 0x11
#define MSG_PRIME_LINE 0x12
#define MSG_STOP 0x13


#define MSG_PIXEL_DATA 0x20
#define MSG_LINE_DATA 0x21
#define MSG_LINE_FINISHED 0x22
#define MSG_INFO 0x23


#define MODE_STREAMING  0x01
#define MODE_ON_DEMAND  0x02

#define CLOCK_INTERNAL 0x01
#define CLOCK_EXTERNAL 0x02




class AbstractArduinoDevice : public SerialDevice
{
   Q_OBJECT

public:
   AbstractArduinoDevice(QObject *parent = 0, QThread *thread = 0);
   ~AbstractArduinoDevice();

   void Init();
   bool IsConnected() { return connected; }


protected:

   virtual void ProcessMessage(const char message, uint32_t param, QByteArray& payload) = 0;
   virtual void SetupAfterConnection() {};

   bool ConnectToDevice(const QString& port);
   void ResetDevice(const QString& port);
   void SendCommand(char command[], float value = NAN);

   template <typename T>
   void SendMessage(char msg, T param, bool require_connection = true);
   void SendMessage(char msg) { SendMessage(msg, uint32_t(0)); }

   bool connected = false;

private:

   QByteArray ReadBytes(int n_bytes, int timeout_ms = 10000);
   void ReadData();

   QByteArray WaitForMessage(char msg, int timeout_ms = 1000);
   QByteArray ProcessMessage(QByteArray msg);
   QByteArray current_message;
   qint64 bytes_left_in_message = 5;

};


template <typename T>
void AbstractArduinoDevice::SendMessage(char msg, T param, bool require_connection)
{
   static_assert(sizeof(param) == 4, "Parameter size must be 4 bytes");

   if (require_connection && !connected)
      return;

   QMutexLocker lk(&connection_mutex);

   char* p = reinterpret_cast<char*>(&param);

   serial_port->write(&msg, 1);
   serial_port->write(p, 4);
   serial_port->flush();
}

