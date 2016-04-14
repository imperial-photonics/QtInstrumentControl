#pragma once

#include "AbstractArduinoDevice.h"
#include <opencv2/core.hpp>

// Admin commands
#define MSG_IDENTIFY 0x49 // 'I'
#define MSG_IDENTITY 0x4A


// Setup commands
#define MSG_SET_MODE 0x01
#define MSG_SET_PIXEL_CLOCK_SOURCE 0x02
#define MSG_SET_LINE_CLOCK_SOURCE 0x03
#define MSG_SET_LAG_TIME 0x04
#define MSG_SET_DWELL_TIME 0x05
#define MSG_SET_NUM_PIXEL 0x06
#define MSG_SET_EXTERNAL_CLOCK_DIVISOR 0x07
#define MSG_SET_TRIGGER_CLOCK_DIVISOR 0x08
#define MSG_SET_GALVO_OFFSET 0x09
#define MSG_SET_GALVO_STEP 0x0A
#define MSG_SET_NUM_FLYBACK_STEPS 0x0B
#define MSG_USE_DIAGNOSTIC_COUNTS 0x0C
#define MSG_SET_TRIGGER_DELAY 0x0D
#define MSG_SET_TRIGGER_DURATION 0x0E

// Triggering commands
#define MSG_TRIGGER 0x10
#define MSG_START_LINE 0x11
#define MSG_PRIME_LINE 0x12
#define MSG_STOP 0x13


#define MSG_PIXEL_DATA 0x20
#define MSG_LINE_DATA 0x21
#define MSG_LINE_FINISHED 0x22
#define MSG_INFO 0x23


#define MODE_STREAMING  0x01
#define MODE_ON_DEMAND  0x02

#define CLOCK_INTERNAL 0x01
#define CLOCK_EXTERNAL 0x02




class ArduinoCounter : public AbstractArduinoDevice
{
    Q_OBJECT

public:
   ArduinoCounter(QObject *parent = 0, QThread *thread = 0);

   void init();

   void GetCount();

   bool IsConnected() { return connected; }

   void SetPMTEnabled(bool enabled);

   void SetStreaming(bool streaming);
   void SetUseExternalPixelClock(bool use_external);
   void SetUseExternalLineClock(bool use_external);
   void SetTriggerDivisor(int trigger_divisor);
   void SetExternalClockDivisor(int ext_clock_divisor);
   void SetDwellTime(double dwell_time_ms);
   void SetPixelsPerLine(int pixels_per_line);
   void SetGalvoStep(int galvo_step);
   void SetGalvoOffset(int galvo_offset);
   void SetNumFlybackSteps(int num_flyback_steps);
   void SetUseDiagnosticCounts(bool use_diagnostic_counts);
   void SetTriggerDelay(double trigger_delay_us);
   void SetTriggerDuration(double trigger_duration_us);

   bool GetStreaming() { return streaming; }
   bool GetUseExternalClock() { return use_external_pixel_clock; }
   bool GetUseExternalLineClock() { return use_external_line_clock; }
   int GetTriggerDivisor() { return trigger_divisor; };
   int GetExternalClockDivisor() { return ext_clock_divisor; }
   double GetDwellTime() { return dwell_time_ms; }
   int GetPixelsPerLine() { return pixels_per_line; }
   int GetGalvoStep() { return galvo_step; }
   int GetGalvoOffset() { return galvo_offset; }
   int GetNumFlybackSteps() { return n_flyback_steps; }
   bool GetUseDiagnosticCounts() { return use_diagnostic_counts; }
   double GetTriggerDelay() { return trigger_delay_us; }
   double GetTriggerDuration() { return trigger_duration_us; }

   int GetCurrentCount() { return current_count; }
   
   int GetNextCount() // should  really do this with mutexes! 
   { 
      QThread::msleep(2 * dwell_time_ms);
      return current_count;
   }

   void StartLine();
   void PrimeLine();
   void Stop();
   
signals:
   void CountUpdated(int count);
   void NewLine(cv::Mat line);
   void LineFinished();
   void PMTEnabled(bool enabled);
   void OverloadOccured();
   void GalvoOffsetChanged(int galvo_offset);

private:

   void SetupAfterConnection();
   void ProcessMessage(const char message, uint32_t param, QByteArray payload);
   const QString getExpectedIdentifier() { return "Photon Counter"; }
   void MonitorCount();

   bool streaming = false;
   double dwell_time_ms = 100;
   bool use_external_line_clock = false;
   bool use_external_pixel_clock = false;
   int ext_clock_divisor = 1;
   int trigger_divisor = 4;
   int pixels_per_line = 1;
   bool use_diagnostic_counts = false;

   int galvo_step = 1;
   int galvo_offset = 0;
   int n_flyback_steps = 0;

   double trigger_delay_us = 0;
   double trigger_duration_us = 1;

   int idx = 0;
   int current_count = 0;

   QTimer* monitor_timer;
};
