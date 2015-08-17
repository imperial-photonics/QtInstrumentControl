#pragma once

#include "SerialDevice.h"
#include <cassert>
#include "QStringList.h"


class GenericNewportController : public SerialDevice
{
   Q_OBJECT
public:

   //enum Units { EncoderCount, MotorStep, Millimeter, Micrometer, Inch, Milliinch, Microinch, Degree, GRadian, Radian, Milliradian, Microradian };
   //QStringList unit_string = { "count", "step", "mm", "um", "inch", "m-inch", "u-inch", "deg", "grad", "rad", "mrad", "urad" };

   GenericNewportController(const QString& controller_type, const QString& stage_type = "", QObject* parent = 0);

   void Init();

   bool ConnectToDevice(const QString& port);
   void ResetDevice(const QString& port) {};

   double GetCurrentPosition();
   double GetTargetPosition();
   void SetTargetPosition(double position);
   void WaitForMotion();

   double GetVelocity();
   void SetVelocity(double velocity);

   double GetAcceleration();
   void SetAcceleration(double acceleration);


   double GetMaximumVelocity();
   double GetMaximumAcceleration();


   void Home();
   void StopMotion();
   void SetMotorState(bool enabled);

   void UpdateCurrentPosition();

   const QString& GetUnits() { return units; }

signals:

   void TargetPositionChanged(double target_position);
   void CurrentPositionChanged(double current_position);
   void VelocityChanged(double velocity);
   void AccelerationChanged(double acceleration);

protected:

   virtual QString SendCommand(int axis, QByteArray command, bool require_connection = true) = 0;
   void GetControllerState();
   void Sync();

   void StartMonitoringMotion();

   template<class T>
   void SendCommand(int axis, QByteArray command, T value);

   virtual void QueryError() = 0;

   QTimer* position_timer;
   QString stage_type;
   QString controller_type;
   
   QString units = "AU";
   double units_per_microstep = 1;

   // Setup baud rates
   QSerialPort::BaudRate baud;
   QByteArray terminator = "\r\n";
   int controller_index = 1;

   bool in_motion = false;
};


template <class T>
void GenericNewportController::SendCommand(int axis, QByteArray command, T value)
{
   if (!connected)
      return;

   QMutexLocker lk(&connection_mutex);

   // Write to stage
   QByteArray b = QByteArray::number(axis);
   b.append(command);
   b.append(QString("%1").arg(value));

   serial_port->write(b);
   serial_port->write(terminator);
   serial_port->flush();
   
   return;
}
