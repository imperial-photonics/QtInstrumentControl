#include "ArduinoCounter.h"

#include <QApplication>

#include <QThread>
#include <algorithm>

using namespace std;

ArduinoCounter::ArduinoCounter(QObject *parent, QThread* thread) :
   SerialDevice(parent, thread)
   
{
   port_description = "Arduino Due";

   monitor_timer = new QTimer(this);
   connect(monitor_timer, &QTimer::timeout, this, &ArduinoCounter::MonitorCount);
   monitor_timer->start(100);

   StartThread();
}

void ArduinoCounter::Init()
{
   SerialDevice::Init();
}

void ArduinoCounter::MonitorCount()
{
   if (!streaming)
      GetCount();
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
   /*
   SendMessage(MSG_IDENTIFY, uint32_t(0), false);
   QByteArray response = WaitForMessage(MSG_IDENTITY);
   if (response != "Photon Counter")
   {
      serial_port->close();
      return false;
   }
   */

   connect(serial_port, &QSerialPort::readyRead, this, &ArduinoCounter::ReadData);

   SetConnected();
   SetDwellTime(dwell_time_ms);
   SetUseExternalPixelClock(use_external_clock);
   SetTriggerDivisor(trigger_divisor);
   SetExternalClockDivisor(ext_clock_divisor);


   emit NewMessage("Connected to Arduino.");
   return true;

}

void ArduinoCounter::SetStreaming(bool streaming_)
{
   streaming = streaming_;
   uint32_t mode = streaming ? MODE_STREAMING : MODE_ON_DEMAND;
   SendMessage(MSG_SET_MODE, mode);
}

void ArduinoCounter::SetDwellTime(double dwell_time_ms_)
{
   dwell_time_ms = dwell_time_ms_;

   float dwell_time_us = dwell_time_ms * 1000.0;
   SendMessage(MSG_SET_DWELL_TIME, dwell_time_us);
}

void ArduinoCounter::SetUseExternalPixelClock(bool use_external_clock_)
{
   use_external_clock = use_external_clock_;
   uint32_t source = use_external_clock ? PIXEL_CLOCK_EXTERNAL : PIXEL_CLOCK_INTERNAL;
   SendMessage(MSG_SET_PIXEL_CLOCK_SOURCE, source);
}
void ArduinoCounter::SetTriggerDivisor(int trigger_divisor_)
{
   trigger_divisor = trigger_divisor_;
   SendMessage(MSG_SET_TRIGGER_CLOCK_DIVISOR, trigger_divisor);
}

void ArduinoCounter::SetExternalClockDivisor(int ext_clock_divisor_)
{
   ext_clock_divisor = ext_clock_divisor_;
   SendMessage(MSG_SET_EXTERNAL_CLOCK_DIVISOR, ext_clock_divisor);

}

void ArduinoCounter::SetPixelsPerLine(int pixels_per_line_)
{
   pixels_per_line = pixels_per_line_;
   SendMessage(MSG_SET_NUM_PIXEL, (quint32) pixels_per_line);
}

void ArduinoCounter::SetGalvoStep(int galvo_step_)
{
   galvo_step = galvo_step_;
   SendMessage(MSG_SET_GALVO_STEP, (quint32) galvo_step);
}

void ArduinoCounter::SetGalvoOffset(int galvo_offset_)
{
   galvo_offset = galvo_offset_;
   SendMessage(MSG_SET_GALVO_OFFSET, galvo_offset);
   emit GalvoOffsetChanged(galvo_offset);
}

void ArduinoCounter::StartLine()
{
   SendMessage(MSG_START_LINE);
}

void ArduinoCounter::Stop()
{
   SendMessage(MSG_STOP);
}

void ArduinoCounter::SetPMTEnabled(bool enabled)
{
   if (!connected)
   {
      emit PMTEnabled(false);
      return;
   }

   // TODO ... set PMT state

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
   SendMessage(MSG_TRIGGER);
}


QByteArray ArduinoCounter::ReadBytes(int n_bytes, int timeout_ms)
{
   char a[10][4];

   char* b = a[4];


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


QByteArray ArduinoCounter::WaitForMessage(char msg, int timeout_ms)
{
   int packet_size = 5;
   QByteArray data;
   
   do
   {
      data = ReadBytes(5);

      if (data.size() == packet_size)
      {
         QByteArray payload = ProcessMessage(data);
         //if (bytes_left_in_message > 0);
         //data.append( ReadBytes() )
         if ((data[0] & 0x7F) == msg)
            return payload;
      }
   } while (data.size() > 0);

   return QByteArray();
}


void ArduinoCounter::ReadData()
{
   QMutexLocker lk(&connection_mutex);


   while (serial_port->bytesAvailable() > bytes_left_in_message)
   {
      QByteArray d = serial_port->read(bytes_left_in_message);
      bytes_left_in_message -= d.size();
      current_message.append(d);

      if (bytes_left_in_message == 0)
         ProcessMessage(current_message);

      if (bytes_left_in_message == 0)
         bytes_left_in_message = 5;
   }
}

QByteArray ArduinoCounter::ProcessMessage(QByteArray data)
{
   unsigned char msg = data[0] & 0x7F;
   char* a = data.data();
   uint32_t param = *reinterpret_cast<uint32_t*>(data.data() + 1);

   QByteArray payload;
   if (data[0] & 0x80) // message has payload
   {
      int total_bytes_expected = param + 5;
      if (data.size() < total_bytes_expected)
      {
         bytes_left_in_message = total_bytes_expected - data.size();
         return payload;
      }
      payload = data.mid(5);

      char b = data[0] & 0x7F;

   }



   switch (msg)
   {
      case MSG_PIXEL_DATA:
      {
         emit CountUpdated(param);
      } break;
      case MSG_LINE_DATA:
      {
         int n_px = payload.size() / 2;
         cv::Mat line(1, n_px, CV_16U, payload.data());

         emit NewLine(line);
      } break;
      case MSG_LINE_FINISHED:
      {
         emit LineFinished();
      } break;
   }

   current_message.clear();

   return payload;
}


ArduinoCounter::~ArduinoCounter()
{
   serial_port->close();
}
