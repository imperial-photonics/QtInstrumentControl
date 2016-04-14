#pragma once

#pragma once

#include "SerialDevice.h"
#include <opencv2/core.hpp>

// Admin commands
#define MSG_IDENTIFY 0x49 // 'I'
#define MSG_IDENTITY 0x4A

class AbstractArduinoDevice : public SerialDevice
{
   Q_OBJECT

public:
   AbstractArduinoDevice(QObject *parent = 0, QThread *thread = 0);
   ~AbstractArduinoDevice();

   void init();
   bool isConnected() { return connected; }

protected:

   virtual void processMessage(const char message, uint32_t param, QByteArray& payload) = 0;
   virtual void setupAfterConnection() {};
   virtual const QString getExpectedIdentifier() = 0;

   template <typename T>
   void sendMessage(char msg, T param, bool require_connection = true);
   void sendMessage(char msg) { sendMessage(msg, uint32_t(0)); }

   bool connected = false;

private:

   bool connectToPort(const QString& port);
   void resetDevice(const QString& port);

   QByteArray readBytes(int n_bytes, int timeout_ms = 10000);
   void readData();

   QByteArray waitForMessage(char msg, int timeout_ms = 1000);
   QByteArray processMessage(QByteArray msg);
   QByteArray current_message;
   qint64 bytes_left_in_message = 5;

};


template <typename T>
void AbstractArduinoDevice::sendMessage(char msg, T param, bool require_connection)
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

