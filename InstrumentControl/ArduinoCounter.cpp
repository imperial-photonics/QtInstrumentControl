#include "ArduinoCounter.h"

#include <QApplication>

#include <QThread>

using namespace std;

ArduinoCounter::ArduinoCounter(QObject *parent) :
   SerialDevice(parent)
   
{
   port_description = "Arduino Due";
}

void ArduinoCounter::ResetDevice(const QString& port_name)
{
   // Reset Arduino by connected and disconnecting
   // with baud rate 1200

   QString m("Trying to reset Arduino on port: ");
   m.append(port_name);
   NewMessage(m);

   QSerialPort port;
   port.setPortName(port_name);
   port.setFlowControl(QSerialPort::HardwareControl);
   port.setReadBufferSize(1000000);
   port.setBaudRate(1200);

   port.open(QIODevice::ReadWrite);
   port.close();

   QThread::msleep(1000); // let arduino reset
}


bool ArduinoCounter::ConnectToDevice(const QString& port)
{
   QMutexLocker lk(&connection_mutex);

   QString m("Trying to connect to Arduino on port: ");
   m.append(port);
   NewMessage(m);

   // Try and open serial port
   if (!OpenSerialPort(port, QSerialPort::HardwareControl, QSerialPort::Baud9600))
      return false;

   // Check that arduino identifies correctly
   QString response = ResponseFromCommand("I");
   if (response != "Photon Counter")
   {
      serial_port->close();
      return false;
   }

   connect(serial_port, &QSerialPort::readyRead, this, &ArduinoCounter::ReadData);
   connected = true;

   SetDwellTime(dwell_time_ms);

   emit NewMessage("Connected to Arduino.");
   return true;

}

void ArduinoCounter::SetDwellTime(double dwell_time_ms_)
{
   // Enforce minimum dwell time
   if (dwell_time_ms_ < 1)
      return;

   dwell_time_ms = dwell_time_ms_;
   SendCommand("P", dwell_time_ms * 1000);
}

void ArduinoCounter::SetPMTEnabled(bool enabled)
{
   if (!connected)
   {
      emit PMTEnabled(false);
      return;
   }

   if (enabled)
      SendCommand("E1");
   else
      SendCommand("E0");

   // need some kind of feedback here really
   emit PMTEnabled(enabled);
}

void ArduinoCounter::SendCommand(char command[], float value)
{   
   if (!connected)
      return;

   QMutexLocker lk(&connection_mutex);

   // Write to arduino
   QByteArray b;
   b.append(command);

   if (!isnan(value)) // not NAN
      b.append((char*)&value, sizeof(value));

   serial_port->write(b);
   serial_port->flush();
}

void ArduinoCounter::GetCount()
{
   if (!connected)
      return;

   QMutexLocker lk(&connection_mutex);

   serial_port->write("X");
}


void ArduinoCounter::ReadData()
{
   QMutexLocker lk(&connection_mutex);

   QByteArray data = serial_port->readAll();
   
   const uint32_t* values = (const uint32_t*) (data.constData());
   int n_values = data.size() / 4;

   int byte = 0;
   for (int i = 0; i < n_values; i++)
   {
      if (values[i] & 0x8000000000000000) // overload signal
      {
         emit OverloadOccured();
         //values[i] = 0;
      }
   }

   uint16_t v = values[n_values - 1] & ((1<<16)-1);

   emit CountUpdated(v);

   QString m("New Message: ");
   m.append(data);

   //emit NewMessage(m);
}

ArduinoCounter::~ArduinoCounter()
{
   if (!connected)
      return;

   serial_port->close();
}
