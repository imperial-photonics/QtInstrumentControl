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
   Q_OBJECT

   enum MotorType { DCMotor, StepperMotor };

public:

   ThorlabsAPTController(const QString& controller_type, const QString& stage_type, QObject* parent = 0);
   ~ThorlabsAPTController();

   void SetPosition(double position);
   void SetToMinimumPosition();
   double GetPosition() { return cur_position; };

   // blocks -> only call this from another thread
   void WaitForMotionComplete();

   const QString& GetUnits() { return units; }

   bool IsConnected() { return connected; }
   bool IsOperational() { return connected & homed & !motion_error; }

   void SetMaxPosition(double max_position);
   double GetMaxPosition() { return max_position; }

   void SetMinPosition(double max_position);
   double GetMinPosition() { return min_position; }

   void SetAllowManualControl(bool allow_manual_control);
   void SetEnforceLimits(bool enforce_limits_);


   void Init();

signals:

   void Operational(); // connected and homed
   void Disconnected();
   void PositionChanged(double position);
   void MoveFinished();
   void MaxPositionChanged(double max_position);
   void MinPositionChanged(double min_position);

protected:

   void ConnectToRotationStage();

   void ConnectToDevice(int dev_index);
   void MonitorConnection();
   
   void SendCommand(uint16_t command, char param1 = 0, char param2 = 0, bool more_data = false);
   void SendCommandWithData(uint16_t command, QByteArray data);
   QByteArray ReadBytes(unsigned int n_bytes, int timeout_ms = 500);
   void ResponseReader();
   bool WaitForStatusUpdate(int timeout_ms);

   void EnablePotSwitch(bool enabled);
   void EnableJogButtons(bool enabled);

   void ProcessHardwareInformationMessage(QDataStream& data);
   void ProcessStatusMessage(QDataStream& data, bool short_version = false);
   void ProcessVelocityParamsMessage(QDataStream& data);
   void ProcessHomeParamsMessage(QDataStream& data);
   
   QTimer* connection_timer;
   void* device = nullptr;
   void* event_handle = nullptr;

   std::thread reader_thread;
   std::mutex status_mutex;
   std::condition_variable status_cv;

   double position_factor;
   double velocity_factor;
   double acceleration_factor;
   MotorType motor_type;
   QString units;
   QString controller_type;

   double target_position = 0;
   double cur_position = 0;
   double cur_velocity = 0;
   double max_velocity = 0;
   double acceleration = 0;

   bool connected = false;
   bool homed = false;
   bool in_motion = false;
   bool motion_error = false;
   bool has_status = false;
   bool watchdog_reset = false;

   double min_position = 0;
   double max_position = 360;
   bool enforce_limits = false;

   std::recursive_mutex send_mutex;

};