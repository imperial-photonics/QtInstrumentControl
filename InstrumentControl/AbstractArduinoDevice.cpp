#include "AbstractArduinoDevice.h"

#include <QApplication>

#include <QThread>
#include <algorithm>
#include <iostream>

using namespace std;

AbstractArduinoDevice::AbstractArduinoDevice(QObject *parent, QThread* thread) :
   SerialDevice(parent, thread)

{
   port_description = "Arduino Due";
}

void AbstractArduinoDevice::init()
{
    connect(this, &AbstractArduinoDevice::newMessage, [](const QString& msg) { std::cout << msg.toStdString() << "\n"; });
   SerialDevice::init();
}


void AbstractArduinoDevice::resetDevice(const QString& port_name)
{
   // Reset Arduino by connected and disconnecting
   // with baud rate 1200

   QString m("Trying to reset Arduino on port: ");
   m.append(port_name);
   newMessage(m);

   QSerialPort port;
   port.setPortName(port_name);
   port.setFlowControl(QSerialPort::HardwareControl);
   port.setReadBufferSize(1000000);
   port.setBaudRate(1200);

   port.open(QIODevice::ReadWrite);
   port.close();

   QThread::msleep(1000); // let arduino reset
}


bool AbstractArduinoDevice::connectToPort(const QString& port)
{
   QMutexLocker lk(&connection_mutex);

   QString m("Trying to connect to Arduino on port: ");
   m.append(port);
   newMessage(m);

   // Try and open serial port
   if (!openSerialPort(port, QSerialPort::HardwareControl, QSerialPort::Baud9600))
      return false;

   // Check that arduino identifies correctly
   
   sendMessage(MSG_IDENTIFY, uint32_t(0), false);
   QByteArray response = waitForMessage(MSG_IDENTITY);
   if (response != getExpectedIdentifier())
   {
      serial_port->close();
      return false;
   }
   

   connect(serial_port, &QSerialPort::readyRead, this, &AbstractArduinoDevice::readData);

   setConnected();
   setupAfterConnection();

   emit newMessage("Connected to Arduino.");
   return true;

}


QByteArray AbstractArduinoDevice::readBytes(int n_bytes, int timeout_ms)
{
   int attempts = timeout_ms / 100;
   QByteArray data;

   QMutexLocker lk(&connection_mutex);

   while (data.size() < n_bytes && attempts > 0)
   {
      serial_port->waitForReadyRead(100);
      QByteArray d = serial_port->read(n_bytes - data.size());
      data.append(d);

      if (d.isEmpty())
         attempts--;
   }

   return data;
}


QByteArray AbstractArduinoDevice::waitForMessage(char msg, int timeout_ms)
{
   int packet_size = 5;
   QByteArray data;


   do
   {
      data = readBytes(packet_size);
      if (data.size() == packet_size)
      {
         QByteArray payload = processMessage(data);
         if ((data[0] & 0x7F) == msg)
            return payload;
      }
   } while (data.size() > 0);

   return QByteArray();
}

/*
Reader function to monitor communications from Arduino.
Builds up packets until they're complete and then dispatches
them to ProcessMessage(...)
*/
void AbstractArduinoDevice::readData()
{
   QMutexLocker lk(&connection_mutex);


   while (serial_port->bytesAvailable() > bytes_left_in_message)
   {
      QByteArray d = serial_port->read(bytes_left_in_message);
      bytes_left_in_message -= d.size();
      current_message.append(d);

      if (bytes_left_in_message == 0)
         processMessage(current_message);

      if (bytes_left_in_message == 0)
         bytes_left_in_message = 5;
   }
}

/*
Interpet a message packet from Arduino
*/
QByteArray AbstractArduinoDevice::processMessage(QByteArray data)
{
   unsigned char msg = data[0] & 0x7F;
   uint32_t param = *reinterpret_cast<uint32_t*>(data.data() + 1);

   QByteArray payload;
   if (data[0] & 0x80) // message has payload
   {
      int total_bytes_expected = param + 5;
      bytes_left_in_message = total_bytes_expected - data.size();
      while (data.size() < total_bytes_expected)
      {
         data.append(readBytes(bytes_left_in_message));
         bytes_left_in_message = total_bytes_expected - data.size();
      }
      payload = data.mid(5);
   }

   processMessage(msg, param, payload);
   current_message.clear();

   return payload;
}


AbstractArduinoDevice::~AbstractArduinoDevice()
{
   serial_port->close();
}
