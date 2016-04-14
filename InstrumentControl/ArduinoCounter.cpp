#include "ArduinoCounter.h"

#include <QApplication>

#include <QThread>
#include <algorithm>
#include <iostream>

using namespace std;

ArduinoCounter::ArduinoCounter(QObject *parent, QThread* thread) :
   AbstractArduinoDevice(parent, thread)
   
{
   startThread();
}

void ArduinoCounter::init()
{
   monitor_timer = new QTimer(this);
   connect(monitor_timer, &QTimer::timeout, this, &ArduinoCounter::MonitorCount);
   monitor_timer->start(50);

   AbstractArduinoDevice::init();
}

void ArduinoCounter::MonitorCount()
{
   if (!streaming)
      GetCount();
}


void ArduinoCounter::SetupAfterConnection()
{
   SetDwellTime(dwell_time_ms);
   SetUseExternalPixelClock(use_external_pixel_clock);
   SetUseExternalLineClock(use_external_line_clock);
   SetTriggerDivisor(trigger_divisor);
   SetExternalClockDivisor(ext_clock_divisor);
   SetNumFlybackSteps(n_flyback_steps);
   SetUseDiagnosticCounts(use_diagnostic_counts);
}

void ArduinoCounter::SetStreaming(bool streaming_)
{
   streaming = streaming_;
   uint32_t mode = streaming ? MODE_STREAMING : MODE_ON_DEMAND;
   sendMessage(MSG_SET_MODE, mode);
}

void ArduinoCounter::SetDwellTime(double dwell_time_ms_)
{
   dwell_time_ms = dwell_time_ms_;

   float dwell_time_us = dwell_time_ms * 1000.0;
   sendMessage(MSG_SET_DWELL_TIME, dwell_time_us);
}

void ArduinoCounter::SetUseExternalPixelClock(bool use_external_clock_)
{
   use_external_pixel_clock = use_external_clock_;
   uint32_t source = use_external_pixel_clock ? CLOCK_EXTERNAL : CLOCK_INTERNAL;
   sendMessage(MSG_SET_PIXEL_CLOCK_SOURCE, source);
}

void ArduinoCounter::SetUseExternalLineClock(bool use_external_clock_)
{
   use_external_line_clock = use_external_clock_;
   uint32_t source = use_external_line_clock ? CLOCK_EXTERNAL : CLOCK_INTERNAL;
   sendMessage(MSG_SET_LINE_CLOCK_SOURCE, source);
}

void ArduinoCounter::SetTriggerDivisor(int trigger_divisor_)
{
   trigger_divisor = trigger_divisor_;
   sendMessage(MSG_SET_TRIGGER_CLOCK_DIVISOR, trigger_divisor);
}

void ArduinoCounter::SetExternalClockDivisor(int ext_clock_divisor_)
{
   ext_clock_divisor = ext_clock_divisor_;
   sendMessage(MSG_SET_EXTERNAL_CLOCK_DIVISOR, ext_clock_divisor);

}

void ArduinoCounter::SetPixelsPerLine(int pixels_per_line_)
{
   pixels_per_line = pixels_per_line_;
   sendMessage(MSG_SET_NUM_PIXEL, (quint32) pixels_per_line);
}

void ArduinoCounter::SetGalvoStep(int galvo_step_)
{
   galvo_step = galvo_step_;
   sendMessage(MSG_SET_GALVO_STEP, (quint32) galvo_step);
}

void ArduinoCounter::SetGalvoOffset(int galvo_offset_)
{
   galvo_offset = galvo_offset_;
   sendMessage(MSG_SET_GALVO_OFFSET, galvo_offset);
   emit GalvoOffsetChanged(galvo_offset);
}

void ArduinoCounter::SetNumFlybackSteps(int n_flyback_steps_)
{
   n_flyback_steps = n_flyback_steps_;
   sendMessage(MSG_SET_NUM_FLYBACK_STEPS, n_flyback_steps);
}

void ArduinoCounter::SetUseDiagnosticCounts(bool use_diagnostic_counts_)
{
   use_diagnostic_counts = use_diagnostic_counts_;
   sendMessage(MSG_USE_DIAGNOSTIC_COUNTS, (int)use_diagnostic_counts);
}

void ArduinoCounter::SetTriggerDelay(double trigger_delay_us_)
{
   trigger_delay_us = trigger_delay_us_;
   sendMessage(MSG_SET_TRIGGER_DELAY, (float)trigger_delay_us);
}

void ArduinoCounter::SetTriggerDuration(double trigger_duration_us_)
{
   trigger_delay_us = trigger_duration_us_;
   sendMessage(MSG_SET_TRIGGER_DURATION, (float)trigger_duration_us_);
}


void ArduinoCounter::StartLine()
{
   sendMessage(MSG_START_LINE);
}

void ArduinoCounter::PrimeLine()
{
   sendMessage(MSG_PRIME_LINE);
}


void ArduinoCounter::Stop()
{
   sendMessage(MSG_STOP);
}

void ArduinoCounter::SetPMTEnabled(bool enabled)
{
   if (!is_connected)
   {
      emit PMTEnabled(false);
      return;
   }

   // TODO ... set PMT state

   // need some kind of feedback here really
   emit PMTEnabled(enabled);
}

void ArduinoCounter::GetCount()
{
   sendMessage(MSG_TRIGGER);
}


/*
   Interpet a message packet from Arduino
*/
void ArduinoCounter::ProcessMessage(const char msg, uint32_t param, QByteArray payload)
{

   switch (msg)
   {
      case MSG_PIXEL_DATA:
      {
         emit CountUpdated(param);
         current_count = param;
      } break;
      case MSG_LINE_DATA:
      {
         int n_px = payload.size() / 2;
         cv::Mat line(1, n_px, CV_16U, payload.data());
         emit NewLine(line);
      } break;
      case MSG_LINE_FINISHED:
      {
         //std::cout << "Line finished\n";
         emit LineFinished();
      } break;
      case MSG_INFO:
      {
         emit newMessage(QString("Arduino Message: %1\n").arg(param));
      }
   }
}
