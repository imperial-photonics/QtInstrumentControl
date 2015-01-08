#pragma once


#include "ThreadedObject.h"
#include <QPointer>
#include <QSerialPort>
#include <QTimer>
#include <QMutex>

#include <cstdint>

class SerialDevice : public ThreadedObject
{
   Q_OBJECT

public:
   SerialDevice(QObject *parent = 0, QThread *thread = 0);
   virtual ~SerialDevice();

   Q_INVOKABLE virtual void Init(); // called on startup
   void Connect();

   virtual bool ConnectToDevice(const QString& port) = 0;
   virtual void ResetDevice(const QString& port) = 0;

signals:
   void Connected();
   void NewMessage(QString const& msg);

protected:

   bool OpenSerialPort(const QString& port, QSerialPort::FlowControl flow_control, int baud_rate);
   QString ResponseFromCommand(const QByteArray& command);

   void SetConnected()
   {
      connected = true;
      emit Connected();
   }

   QByteArray ReadUntilTerminator(int timeout_ms);
   void WriteWithTerminator(const QByteArray& command);

   void ErrorOccurred(QSerialPort::SerialPortError error);
   void Disconnected();


   QString port_description;
   QSerialPort* serial_port;
   QTimer* connection_timer;
   QMutex connection_mutex;

   static QMutex port_detection_mutex;

   bool connected;
   bool shutdown;

   QByteArray terminator = "\r\n";

};
