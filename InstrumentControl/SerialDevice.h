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

   virtual void init(); // called on startup
   void connectToDevice();

   virtual bool connectToPort(const QString& port) = 0;
   virtual void resetDevice(const QString& port) = 0;

   bool isConnected() { return is_connected; }

signals:
   void connected();
   void newMessage(QString const& msg);

protected:

   bool openSerialPort(const QString& port, QSerialPort::FlowControl flow_control, int baud_rate);
   QString responseFromCommand(const QByteArray& command);

   void setConnected()
   {
      is_connected = true;
      emit connected();
   }

   QByteArray readUntilTerminator(int timeout_ms);
   void writeWithTerminator(const QByteArray& command);

   void errorOccurred(QSerialPort::SerialPortError error);
   void disconnected();


   QString port_description;
   QSerialPort* serial_port;
   QTimer* connection_timer;
   QMutex connection_mutex;

   static QMutex port_detection_mutex;

   bool is_connected;
   bool shutdown;

   QByteArray terminator = "\r\n";

};
