#include "GenericNewportController.h"

#include <QThread>
#include <QSerialPortInfo>
#include <iostream>

using namespace std;

GenericNewportController::GenericNewportController(const QString& controller_type, const QString& stage_type, QObject* parent) :
SerialDevice(parent),
controller_type(controller_type),
stage_type(stage_type)
{
}

void GenericNewportController::Init()
{
   SerialDevice::Init();

   position_timer = new QTimer(this);
   position_timer->setInterval(500);

   connect(position_timer, &QTimer::timeout, this, &GenericNewportController::UpdateCurrentPosition);
   position_timer->start();
}

bool GenericNewportController::ConnectToDevice(const QString& port)
{
   QMutexLocker lk(&connection_mutex);

   QString m = QString("Trying to connect to Newport stage on port: %1").arg(port);
   std::cout << m.toStdString() << "\n";
   NewMessage(m);

   // Try and open serial port
   if (!OpenSerialPort(port, QSerialPort::SoftwareControl, baud))
      return false;

   // Check that device identifies correctly
   QString response = SendCommand(controller_index, "VE", false); // false == don't require connection
   if (!response.contains(controller_type))
   {
      serial_port->close();
      return false;
   }

   // Check that stage identifies correctly if specified
   if (!stage_type.isEmpty())
   {
      response = SendCommand(controller_index, "ID", false); // false == don't require connection
      if (response != stage_type)
      {
         serial_port->close();
         return false;
      }
   }

   connected = true;

   GetControllerState();

   Sync();

   emit NewMessage("Connected to Newport Stage.");
   return true;
}

void GenericNewportController::UpdateCurrentPosition()
{
   if (connected)
   {
      double current_position = GetCurrentPosition();
      emit CurrentPositionChanged(current_position);

      GetControllerState();
   }
}

void GenericNewportController::SetMotorState(bool state)
{
   SendCommand(controller_index, "MM", static_cast<int>(state));
}

double GenericNewportController::GetCurrentPosition()
{
   return SendCommand(controller_index, "TP").toDouble() * units_per_microstep;
}

double GenericNewportController::GetTargetPosition()
{
   return SendCommand(controller_index, "TH").toDouble() * units_per_microstep;
}

void GenericNewportController::SetTargetPosition(double position)
{
   in_motion = true;

   SendCommand(controller_index, "PA", position / units_per_microstep);
   QueryError();

   //StartMonitoringMotion();
}

double GenericNewportController::GetVelocity()
{
   double velocity = SendCommand(controller_index, "VA").toDouble();
   QueryError();
   return velocity;
}

void GenericNewportController::SetVelocity(double velocity)
{
   SendCommand(controller_index, "VA", velocity);
   QueryError();
}

double GenericNewportController::GetAcceleration()
{
   double acceleration = SendCommand(controller_index, "AC").toDouble();
   QueryError();
   return acceleration;
}

void GenericNewportController::SetAcceleration(double acceleration)
{
   SendCommand(controller_index, "AC", acceleration);
   QueryError();
}

void GenericNewportController::StopMotion()
{
   SendCommand(controller_index, "ST");
   QueryError();
}

void GenericNewportController::Home()
{
   if (connected)
   {
      SendCommand(controller_index, "RS"); // reset;
      QThread::sleep(10); // wait for reset

      while (SendCommand(controller_index, "TS").isEmpty()) 
         QThread::msleep(500);

      SendCommand(controller_index, "OR"); // home;
      WaitForMotion();

      Sync();
      

   }
}

/*
   Update controller values from device
*/
void GenericNewportController::Sync()
{
   emit CurrentPositionChanged(GetCurrentPosition());
//   emit TargetPositionChanged(GetTargetPosition());
   emit VelocityChanged(GetVelocity());
   emit AccelerationChanged(GetAcceleration());
}


void GenericNewportController::WaitForMotion()
{
   while (in_motion)
   {
      QThread::msleep(100);
   }

   //TODO

   /*
   bool is_moving = true;

   while (is_moving)
   {
      QString ts_return = SendCommand(controller_index, "TS");

      if (ts_return.size() != 6) // malformed response
         return;

      QString state = ts_return.right(2);

      is_moving = (state == "28" || state == "1E" || state == "1F"); // moving or homing
   }
   */
}

void GenericNewportController::GetControllerState()
{
   QString ts_return = SendCommand(controller_index, "TS");

   if (ts_return.size() != 6) // malformed response
      return;

   QString error = ts_return.left(4);
   QString state = ts_return.right(2);

   in_motion = (state == "28" || state == "1E" || state == "1F"); // moving or homing

   /*
    if (state == "10") // not referenced from stage error
   {
      SendCommand(controller_index, "ZX", 2); // Update stage information
      SendCommand(controller_index, "ZX", 3); // Enable stage check

      SendCommand(controller_index, "PW", 1); // Enter config state
      SendCommand(controller_index, "SR", 200); // Set stage limit to 200
      SendCommand(controller_index, "PW", 0); // Exit config state


//      SendCommand(controller_index, "RS"); // Reset
      GetControllerState();
   }
   else if (state == "0C") // not referenced from configuration
   {
      SendCommand(1, "OR"); // Execute home search
   }
   */
}
