#pragma once

#include <QObject>
#include <QByteArray>
#include <QTimer>
#include "ThreadedObject.h"

#include <thread>
#include <mutex>
#include <condition_variable>

class ThorlabsAPTController : public ThreadedObject
{
   enum MotorType { DCMotor, StepperMotor };

   Q_OBJECT
public:

   ThorlabsAPTController(const QString& controller_type, const QString& stage_type, QObject* parent = 0);
   ~ThorlabsAPTController();

   void SetPosition(double position);
   double GetPosition() { return cur_position; };

   const QString& GetUnits() { return units; }

   void Init();

protected:

   void ConnectToDevice(int dev_index);
   void MonitorConnection();
   
   void SendCommand(uint16_t command, char param1 = 0, char param2 = 0, bool more_data = false);
   void SendCommandWithData(uint16_t command, QByteArray data, char param1 = 0, char param2 = 0);
   QByteArray ReadBytes(unsigned int n_bytes, int timeout_ms = 500);
   void ResponseReader();
   void WaitForStatusUpdate();

   void ProcessHardwareInformationMessage(QDataStream& data);
   void ProcessStatusMessage(QDataStream& data, bool short_version = false);
   void ProcessVelocityParamsMessage(QDataStream& data);
   
   QTimer* connection_timer;
   void* device = nullptr;
   void* event_handle = nullptr;

   std::thread reader_thread;
   std::mutex status_mutex;
   std::condition_variable status_cv;

   double position_factor;
   double velocity_factor;
   double acceleration_factor;
   double position_zero = 0;
   MotorType motor_type;
   QString units;
   QString controller_type;

   double cur_position = 0;
   double cur_velocity = 0;
   double max_velocity = 0;
   double acceleration = 0;

   bool connected = false;
   bool homed = false;
   bool in_motion = false;
   bool has_status = false;
   bool watchdog_reset = false;

};