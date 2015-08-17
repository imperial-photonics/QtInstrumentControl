#include "SerialDevice.h"

#include <QSerialPortInfo>
#include <QDateTime>

#include <iostream>
#include <cstdint>

using namespace std;

QMutex SerialDevice::port_detection_mutex;

SerialDevice::SerialDevice(QObject *parent, QThread *thread) :
ThreadedObject(parent, thread),
connection_mutex(QMutex::Recursive),
connected(false),
shutdown(false)
{
}

SerialDevice::~SerialDevice()
{
   shutdown = true;
}

void SerialDevice::Init()
{
   serial_port = new QSerialPort(this);
   // Setup reconnection timer
   connection_timer = new QTimer(this);
   connection_timer->setInterval(2000);
   connection_timer->setSingleShot(true);
   connect(connection_timer, &QTimer::timeout, this, &SerialDevice::Connect);

   Connect();
}

void SerialDevice::Connect()
{
   QMutexLocker lk(&port_detection_mutex);

   QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

   for (auto port : ports)
   {
      std::string description = port.description().toStdString();
      std::string manufacturer = port.manufacturer().toStdString();

      if (!port.isBusy() && (port_description.isEmpty() || port.description() == port_description))
      {
         // Try to connect, return if we succesfully connected
         if (ConnectToDevice(port.portName()))
            return;
         //else
         //resetArduino(port.portName());
      }
   }

   // Couldn't connect, try autoconnection later
   connection_timer->start();
   //NewMessage("Could not connect to device.");
}



bool SerialDevice::OpenSerialPort(const QString& port, QSerialPort::FlowControl flow_control, int baud_rate)
{
   QMutexLocker lk(&connection_mutex);

   if (serial_port != nullptr)
      delete serial_port;

   serial_port = new QSerialPort(this);

   serial_port->setPortName(port);
   serial_port->setFlowControl(flow_control);
   serial_port->setBaudRate(baud_rate);

   serial_port->setReadBufferSize(0);

   if (serial_port->open(QIODevice::ReadWrite) == false)
      return false;

   connect(serial_port, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error), this, &SerialDevice::ErrorOccurred);
   connect(serial_port, &QSerialPort::aboutToClose, this, &SerialDevice::Disconnected);

   return true;
}

/*
Get a response from the device to a command string.
*/
QString SerialDevice::ResponseFromCommand(const QByteArray& command)
{
   QMutexLocker lk(&connection_mutex);

   WriteWithTerminator(command);
   return ReadUntilTerminator(1000);
}

void SerialDevice::WriteWithTerminator(const QByteArray& command)
{
   serial_port->write(command);
   serial_port->write(terminator);
   serial_port->flush();

   while (serial_port->waitForBytesWritten(100)) {};

   //std::cout << "Command: " << command.constData() << "\n";
}

QByteArray SerialDevice::ReadUntilTerminator(int timeout_ms)
{
   QByteArray data;
   qint64 tstart = QDateTime::currentMSecsSinceEpoch();
   qint64 telapsed = 0;
   bool finished = false;
   bool something_read = false;
   
   serial_port->waitForReadyRead(1000);

   do
   {
      something_read = false;
      char c = 0;
      while (serial_port->getChar(&c))
      {
         if (c == '\n' || c == '\r') // terminate line
         {
            finished = true;
         }
         else // ignore carrage returns
         {
            // Once we've got some data wait for the rest
            data.append(c);
               something_read = true;
         }
      }

      if (finished)
         break;
      else
      {
         QThread::msleep(50);
         serial_port->waitForReadyRead(1000);
      }
   } while (something_read);

   //std::cout << "Response: " << data.constData() << "\n";

   return data;
}

void SerialDevice::ErrorOccurred(QSerialPort::SerialPortError error)
{
   QMutexLocker lk(&connection_mutex);

   // Display error message
   //cout << serial_port->errorString().toStdString() << "\n";
   //emit NewMessage(serial_port->errorString());

   int err = serial_port->error();

   // If we couldn't write close and try to connect again
   // Probably means the Arduino was disconnected
   if (!shutdown)
      if (err == QSerialPort::WriteError || err == QSerialPort::DeviceNotFoundError)
      {
         serial_port->close();
         connected = false;
         connection_timer->start();
      }

}

void SerialDevice::Disconnected()
{
   // Make sure we don't try to keep writing when disconnected.
   emit NewMessage("Device disconnected.");
   connected = false;
}
