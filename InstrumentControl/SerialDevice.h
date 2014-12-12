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
   SerialDevice(QObject *parent = 0);
   virtual ~SerialDevice() {};

   virtual void Init(); // called on startup
   void Connect();

   virtual bool ConnectToDevice(const QString& port) = 0;
   virtual void ResetDevice(const QString& port) = 0;

signals:
   void NewMessage(QString const& msg);

protected:

   bool OpenSerialPort(const QString& port, QSerialPort::FlowControl flow_control, int baud_rate);
   QString ResponseFromCommand(const QByteArray& command);
   
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

   QByteArray terminator = "\r\n";

};
