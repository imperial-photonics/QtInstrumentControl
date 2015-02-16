#include "ThorlabsAPTController.h"
#include "ThorlabsAPTCommands.h"
#include <windows.h>
#include "FTD2XX.h"
#include <iostream>
#include <vector>

#include <QDataStream>

using namespace std;

void Check(unsigned long status)
{
   if (status != FT_OK)
   {
      cout << "Thorlabs APT Controller error\n";
      throw exception("Error with Thorlabs APT Controller", status);
   }
}

vector<FT_DEVICE_LIST_INFO_NODE> GetAPTDevices()
{
   DWORD n_dev;

   // create the device information list
   Check(FT_CreateDeviceInfoList(&n_dev));
   printf("Number of devices is %d\n", n_dev);

   // allocate storage for list based on numDevs
   vector<FT_DEVICE_LIST_INFO_NODE> dev_info(n_dev);

   if (n_dev > 0)
   {
      // get the device information list
      Check(FT_GetDeviceInfoList(dev_info.data(), &n_dev));

      for (auto& dev : dev_info)
      {
         printf(" Flags=0x%x\n", dev.Flags);
         printf(" Type=0x%x\n", dev.Type);
         printf(" ID=0x%x\n", dev.ID);
         printf(" LocId=0x%x\n", dev.LocId);
         printf(" SerialNumber=%s\n", dev.SerialNumber);
         printf(" Description=%s\n", dev.Description);
      }
   }

   return dev_info;
}

/*
   Information on communicating using FTD2xx:
   http://www.ftdichip.com/Support/Documents/ProgramGuides/D2XX_Programmer's_Guide(FT_000071).pdf
*/



FT_HANDLE GetAPTDeviceByType(const QString& type)
{
   vector<FT_DEVICE_LIST_INFO_NODE> devices = GetAPTDevices();

   for (auto& dev : devices)
   {
      if (dev.Description == type)
         return dev.ftHandle;
   }

   return nullptr;
}


ThorlabsAPTController::ThorlabsAPTController(const QString& controller_type, const QString& stage_type, QObject* parent) :
   ThreadedObject(parent),
   controller_type(controller_type)
{
   // Setup conversion factors based on selected stage
   if (controller_type == "TDC001")
   {
      motor_type = DCMotor;
      double T = 2048.0 / 6.0e6;
      double encoder_count;

      if (stage_type == "MTS25-Z8" || stage_type == "MTS50-Z8" || stage_type.left(2) == "Z8")
      {
         units = "mm";
         encoder_count = 34304;
      }
      else if (stage_type == "PRM1-Z8")
      {
         encoder_count = 1919.64;
         units = "deg";
      }
      else if (stage_type.left(2) == "Z6")
      {
         encoder_count = 24600;
         units = "mm";
      }
      else
         throw(std::exception("Unrecognised stage type"));

      position_factor = encoder_count;
      velocity_factor = encoder_count * T * 65536;
      acceleration_factor = encoder_count * T * T * 65536;
   }
   else if (controller_type == "TBD001" || controller_type.left(3) == "BBD")
   {
      motor_type = DCMotor;
      double T = 102.4e-6;
      double encoder_count;

      if (stage_type == "DDSM100")
         encoder_count = 2000;
      if (stage_type == "DDS220" || stage_type == "DDS300" || stage_type == "DDS600" || stage_type == "MLS203")
         encoder_count = 20000;
      else
         throw(std::exception("Unrecognised stage type"));

      position_factor = encoder_count;
      velocity_factor = encoder_count * T * 65536;
      acceleration_factor = encoder_count * T * T * 65536;
   }
   else
   {
      // Need to add support for additional controllers here - 
      // see APT communications protocol document
      throw(std::exception("Unrecognised Thorlabs APT controller, only DC motors currently supported"));
   }

   StartThread();
}

ThorlabsAPTController::~ThorlabsAPTController()
{
   connected = false;
   emit Disconnected();

   if (reader_thread.joinable())
      reader_thread.join();
}

void ThorlabsAPTController::ConnectToDevice(int dev_index)
{
   if (device != nullptr)
   {
      /*
      try
      {
         SendCommand(MGMSG_MOT_SUSPEND_ENDOFMOVEMSGS, 1);
         SendCommand(MGMSG_HW_STOP_UPDATEMSGS);
      } catch (std::exception e)
      { }
      */

      FT_Close(device);
      device = nullptr;
   }

   if (event_handle != nullptr)
   {
      CloseHandle(event_handle);
      event_handle = nullptr;
   }
   

   ULONG baud_rate = 115200;
   int purge_dwell_time = 50;

   // TODO: open correct device index not just first T-Cube

   GetAPTDevices();

   Check(FT_OpenEx("APT TDC001 T-Cube (Rev 2)", FT_OPEN_BY_DESCRIPTION, &device)); // TODO

   Check(FT_SetBaudRate(device, baud_rate));
   Check(FT_SetDataCharacteristics(device, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE));

   // Pre purge dwell 50ms.
   QThread::msleep(purge_dwell_time);
   Check(FT_Purge(device, FT_PURGE_RX | FT_PURGE_TX));
   // Post purge dwell 50ms.
   QThread::msleep(purge_dwell_time);

   Check(FT_ResetDevice(device));
   Check(FT_SetFlowControl(device, FT_FLOW_RTS_CTS, 0, 0));
   Check(FT_SetRts(device));

   // Setup signalling event
   bool manual_reset = false;
   bool signalled = false;
   event_handle = CreateEvent(NULL, manual_reset, signalled, "");

   Check(FT_SetEventNotification(device, FT_EVENT_RXCHAR, event_handle));

   connected = true;
}

void ThorlabsAPTController::MonitorConnection()
{
   if (connected && watchdog_reset)
   {
      watchdog_reset = false;
   }
   else
   {

      cout << "Not connected... attempting to connect to APT controller\n";

      connected = false;
      homed = false;
      Disconnected();

      if (reader_thread.joinable())
         reader_thread.join();

      try
      {
         ConnectToDevice(0);

         reader_thread = std::thread(&ThorlabsAPTController::ResponseReader, this);
         QThread::msleep(100);

         SendCommand(MGMSG_HW_REQ_INFO); // Request hardware info
         SendCommand(MGMSG_MOD_SET_CHANENABLESTATE, 1, 1); // Enable motor on channel 1
         SendCommand(MGMSG_MOT_RESUME_ENDOFMOVEMSGS, 1, 0); // Make sure we get responses
         SendCommand(MGMSG_HW_START_UPDATEMSGS, 2); // Update messages 2Hz update rate
         SendCommand(MGMSG_MOT_REQ_DCSTATUSUPDATE, 1);
        
         bool a = WaitForStatusUpdate(1000);

         // Make sure device is homed
         if (!homed)
         {
            std::cout << "   Homing stage...\n";
            SendCommand(MGMSG_MOT_MOVE_HOME, 1);
         }
         else
         {
            QThread::msleep(100);
            emit Operational();
         }

      }
      catch (std::exception e)
      {
         std::cout << "Could not connect to Thorlabs APT controller : \n  ";
         std::cout << e.what() << "\n\n";
      }
   }
   // Setup timer to monitor connection
   connection_timer->start(5000);
}

bool ThorlabsAPTController::WaitForStatusUpdate(int timeout_ms)
{
   unique_lock<mutex> lk(status_mutex);
   has_status = false;
   //status_cv.wait_for(lk, std::chrono::milliseconds(timeout_ms), [this]{ return has_status; });
   status_cv.wait(lk, [this]{ return has_status; });

   return has_status;
}

void ThorlabsAPTController::Init()
{
   connection_timer = new QTimer(this);
   connection_timer->setSingleShot(true);
   connect(connection_timer, &QTimer::timeout, this, &ThorlabsAPTController::MonitorConnection);
   

   // Attempt to connect to controller
   MonitorConnection();
}

void ThorlabsAPTController::EnablePotSwitch(bool enabled)
{
   QByteArray data;
   QDataStream ds(&data, QIODevice::WriteOnly);
   ds.setByteOrder(QDataStream::LittleEndian);

   quint16 channel = 1;
   const quint16 *win = TDC001_pot_win_params;
   const qint32 *vel = enabled ? TDC001_pot_vel_params : TDC001_pot_disabled_vel_params;

   ds << channel << win[0] << vel[0] << win[1] << vel[1] << win[2] << vel[2] << win[3] << vel[3];

   SendCommandWithData(MGMSG_MOT_SET_POTPARAMS, data);
}


void ThorlabsAPTController::EnableJogButtons(bool enabled)
{
   QByteArray data;
   QDataStream ds(&data, QIODevice::WriteOnly);
   ds.setByteOrder(QDataStream::LittleEndian);

   quint16 channel = 1;
   quint16 mode = enabled ? 0x01 : 0x00;
   qint32 position = 0;
   
   ds << channel << mode << position << position;

   SendCommandWithData(MGMSG_MOT_SET_BUTTONPARAMS, data);
}

void ThorlabsAPTController::SetToMinimumPosition()
{
   SetPosition(min_position);
}

void ThorlabsAPTController::SetPosition(double position_)
{
   if (!connected || !homed)
      return;

   if (enforce_limits)
   {
      if (position_ < min_position)
         position_ = min_position;
      if (position_ > max_position)
         position_ = max_position;
   }

   std::cout << "Setting position: " << position_ << "\n";
   target_position = position_;

   QByteArray data;
   QDataStream ds(&data, QIODevice::WriteOnly);
   ds.setByteOrder(QDataStream::LittleEndian);
   
   quint16 channel = 1;
   qint32 position = position_ * position_factor;

   ds << channel << position;

   SendCommandWithData(MGMSG_MOT_MOVE_ABSOLUTE, data);
}

void ThorlabsAPTController::WaitForMotionComplete()
{
   while (in_motion || (abs(cur_position-target_position) > 1)) // second condition just makes sure we wait until we start moving
   { 
      QThread::msleep(5); 
   }
}

void ThorlabsAPTController::SetMaxPosition(double max_position_)
{
   if (max_position_ == max_position)
      return;

   if (enforce_limits && cur_position > max_position_)
      SetPosition(max_position_);

   max_position = max_position_;
   emit MaxPositionChanged(max_position);
   //enforce_limits = true;

}

void ThorlabsAPTController::SetMinPosition(double min_position_)
{
   if (min_position_ == min_position)
      return;

   if (enforce_limits && cur_position < min_position_)
      SetPosition(min_position_);

   min_position = min_position_;
   emit MinPositionChanged(min_position);
   //enforce_limits = true;

}

void ThorlabsAPTController::SetEnforceLimits(bool enforce_limits_)
{ 
   enforce_limits = enforce_limits_; 

   if (enforce_limits)
   {
      if (cur_position < min_position)
         SetPosition(min_position);
      if (cur_position > max_position)
         SetPosition(max_position);
   }
}

void ThorlabsAPTController::SetAllowManualControl(bool allow_manual_control)
{
   EnableJogButtons(allow_manual_control);
   EnablePotSwitch(allow_manual_control);
}


void ThorlabsAPTController::SendCommand(uint16_t command, char param1, char param2, bool more_data)
{
   lock_guard<recursive_mutex> lk(send_mutex);

   char src = 0x01; // host PC
   char dest = 0x50; // USB unit

   if (more_data)
      dest |= 0x80;
    
   char* cmd = reinterpret_cast<char*>(&command);
 
   unsigned char tx[6];
   tx[0] = cmd[0];
   tx[1] = cmd[1];
   tx[2] = param1;
   tx[3] = param2;
   tx[4] = dest;
   tx[5] = src;

   DWORD bytes_written;
   Check(FT_Write(device, reinterpret_cast<void*>(tx), 6, &bytes_written));

   if (bytes_written != 6)
      int a = 1;
}

void ThorlabsAPTController::SendCommandWithData(uint16_t command, QByteArray data)
{
   lock_guard<recursive_mutex> lk(send_mutex);

   char size = data.size();
   SendCommand(command, size, 0, true);

   DWORD bytes_written;

   // Get char* pointer from data - we need to strip const
   const char* data_c_ptr = data;
   char* data_ptr = const_cast<char*>(data_c_ptr);

   Check(FT_Write(device, data_ptr, data.size(), &bytes_written));
}

QByteArray ThorlabsAPTController::ReadBytes(unsigned int n_bytes, int timeout_ms)
{
   DWORD EventDWord;
   DWORD n_rx_bytes = 0, n_tx_bytes = 0;

   // Sometime later, block the application thread by waiting on the event, then when the event has
   // occurred, determine the condition that caused the event, and process it accordingly.
   //DWORD s = WaitForSingleObject(event_handle, timeout_ms);

   FT_GetStatus(device, &n_rx_bytes, &n_tx_bytes, &EventDWord);

   int attempts = timeout_ms / 5;
   while (n_rx_bytes < n_bytes && attempts-- > 0)
   {
      FT_GetStatus(device, &n_rx_bytes, &n_tx_bytes, &EventDWord);
      QThread::msleep(5);
   }

   if (n_rx_bytes >= n_bytes)
   {
      QByteArray data(n_bytes, Qt::Uninitialized);

      DWORD bytes_read;
      FT_Read(device, data.data(), n_bytes, &bytes_read);
      data.truncate(bytes_read);

      return data;
   }

   return QByteArray();
}

void ThorlabsAPTController::ResponseReader()
{
   while (connected)
   {
      QByteArray header = ReadBytes(6);

      if (header.size() == 6)
      {
         QDataStream ds(&header, QIODevice::ReadOnly);
         ds.setByteOrder(QDataStream::LittleEndian);

         quint16 response;
         quint8 param1, param2, dest, src;
         ds >> response >> param1 >> param2 >> dest >> src;

         if (dest & 0x80) // Messages with data packets
         {
            QByteArray data = ReadBytes(param1);

            if (data.size() == param1)
            {

               QDataStream ds(&data, QIODevice::ReadOnly);
               ds.setByteOrder(QDataStream::LittleEndian);

               quint16 channel;
               int a;
               switch (response)
               {
               case MGMSG_HW_RICHRESPONSE:
                  a = 1;
                  // TODO: ERROR OCCURRED

               case MGMSG_HW_GET_INFO:
                  ProcessHardwareInformationMessage(ds);
                  break;

               case MGMSG_MOT_GET_POSCOUNTER:
                  qint32 position;
                  ds >> channel >> position;
                  break;

               case MGMSG_MOT_GET_ENCCOUNTER:
                  qint32 encoder;
                  ds >> channel >> encoder;
                  break;

               case MGMSG_MOT_GET_VELPARAMS:
                  ProcessVelocityParamsMessage(ds);
                  break;

                  // all these messages are followed by status message
               case MGMSG_MOT_MOVE_COMPLETED:
               case MGMSG_MOT_MOVE_STOPPED:
                  emit MoveFinished();
               case MGMSG_MOT_GET_DCSTATUSUPDATE:
                  ProcessStatusMessage(ds);
                  break;

               case MGMSG_MOT_GET_STATUSBITS:
                  ProcessStatusMessage(ds, true);
                  break;

               case MGMSG_MOT_GET_POTPARAMS:

                  quint16 channel, wnd[4];
                  qint32 vel[4];

                  ds >> channel >> wnd[0] >> vel[0]
                     >> wnd[1] >> vel[1]
                     >> wnd[2] >> vel[2]
                     >> wnd[3] >> vel[3];


                  break;

                  // The following messages are currently ignored
               case MGMSG_HUB_GET_BAYUSED:
               case MGMSG_MOT_GET_JOGPARAMS:
               case MGMSG_MOT_GET_GENMOVEPARAMS:
               case MGMSG_MOT_GET_MOVERELPARAMS:
               case MGMSG_MOT_GET_MOVEABSPARAMS:
               case MGMSG_MOT_GET_HOMEPARAMS:
               case MGMSG_MOT_GET_LIMSWITCHPARAMS:
               case MGMSG_MOT_GET_DCPIDPARAMS:
               case MGMSG_MOT_GET_AVMODES:
               case MGMSG_MOT_GET_BUTTONPARAMS:
                  break;

               }
            }
         }
         else // Header only messages
         {
            int a;
            switch (response)
            {
            case MGMSG_MOT_MOVE_HOMED:
               std::cout << "   Homing complete.\n";
               homed = true;
               emit Operational();
               break;

            case MGMSG_HW_DISCONNECT:
               connected = false;
               emit Disconnected();
               break;

            case MGMSG_HW_RESPONSE:
               a = 1;
               // TODO: ERROR OCURRED
               break;

               // The following messages are currently ignored
            case MGMSG_MOD_GET_CHANENABLESTATE:
               break;
            }
         }

      } 
   }
}

void ThorlabsAPTController::ProcessStatusMessage(QDataStream& ds, bool short_version)
{
   watchdog_reset = true;

   quint16 channel, velocity, empty;
   qint32 position, status;

   if (short_version)
   {
      ds >> channel >> status;
   }
   else
   {
      double intpart;

      ds >> channel >> position >> velocity >> empty >> status;

      cur_position = position / position_factor;
      cur_position = 360.0 * modf(cur_position / 360.0, &intpart);
      cur_velocity = velocity / velocity_factor;

      emit PositionChanged(cur_position);

      //cout << "Last position:" << cur_position << "\n";
      //cout << "Last velocity:" << cur_velocity << "\n";
   }

   // Read status bits;
   homed = status & 0x400;
   bool in_motion_ = status & (0x10 | 0x20 | 0x40 | 0x80 | 0x200);

   if (in_motion & !in_motion_) // just stopped moving
      emit MoveFinished();

   in_motion = in_motion_;

   bool forward_hw_limit = status & 0x1;
   bool rev_hw_limit = status & 0x2;
   bool homing = status & 0x200;
   bool tracking = status & 0x1000;
   bool settled = status & 0x2000;
   bool motor_overcurrent = status & 0x01000000;

   motion_error = status & 0x4000;

   if (motion_error)
      std::cout << "Thorlabs APT Motion Error!\n";

   SendCommand(MGMSG_MOT_ACK_DCSTATUSUPDATE);

   if (enforce_limits & homed & !in_motion)
   {
      int tol = 1;
      if (cur_position > (max_position+tol))
         SetPosition(max_position);
      if (cur_position < (min_position-tol))
         SetPosition(min_position);
   }


   lock_guard<mutex> lk(status_mutex);
   has_status = true;
   status_cv.notify_all();
}

void ThorlabsAPTController::ProcessHardwareInformationMessage(QDataStream& ds)
{
   qint32 serial_no;
   QByteArray model_no(8, Qt::Uninitialized);
   quint16 type;
   QByteArray firmware_version(4, Qt::Uninitialized);
   QByteArray notes(48, Qt::Uninitialized);
   QByteArray empty(12, Qt::Uninitialized);
   quint16 hw_version, mod_state, n_chans;

   ds >> serial_no;
   ds.readRawData(model_no.data(), model_no.size());
   ds >> type;
   ds.readRawData(firmware_version.data(), firmware_version.size());
   ds.readRawData(notes.data(), notes.size());
   ds.readRawData(empty.data(), empty.size());
   ds >> hw_version >> mod_state >> n_chans;

   QString model(model_no);
   if (model != controller_type)
      throw(std::exception("APT controller is not the type expected"));
}

void ThorlabsAPTController::ProcessVelocityParamsMessage(QDataStream& ds)
{
   qint32 start_velocity_, acceleration_, max_velocity_;
   ds >> start_velocity_ >> acceleration_ >> max_velocity_;

   acceleration = acceleration_ / acceleration_factor;
   max_velocity = max_velocity_ / velocity_factor;
}
