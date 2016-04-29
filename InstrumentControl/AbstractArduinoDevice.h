#pragma once

#pragma once

#include "SerialDevice.h"
#include <opencv2/core.hpp>
#include <QVariant>

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

protected:

   virtual void processMessage(const char message, uint32_t param, QByteArray& payload) = 0;
   virtual void setupAfterConnection() {};
   virtual const QString getExpectedIdentifier() = 0;

   Q_INVOKABLE void sendMessage(char msg, QVariant param, bool require_connection = true);
   void sendMessage(char msg) { sendMessage(msg, 0); }

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



