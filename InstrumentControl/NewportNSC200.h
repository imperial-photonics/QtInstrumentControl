#pragma once

#include "GenericNewportController.h"
#include <QThread>

class NewportNSC200 : public GenericNewportController
{
   Q_OBJECT
public:

   NewportNSC200(const QString& stage_type, QObject* parent = 0) :
      GenericNewportController("NSC200", stage_type, parent)
   {
      baud = QSerialPort::Baud19200;

      units_per_microstep = 1.0 / 64;

      if (stage_type == "NSR1")
         units = "deg";

      StartThread();
   }

protected:
   
   QString SendCommand(int axis, QByteArray command, bool require_connection = true)
   {
      if (require_connection && !connected)
         return 0;

      QMutexLocker lk(&connection_mutex);

      // Write to stage
      QByteArray b = QByteArray::number(axis).append(command).append("?");

      serial_port->readAll();
      WriteWithTerminator(b);
      QByteArray response = ReadUntilTerminator(1000);

      b.append(" ");

      int response_length = response.size() - b.size();

      if (response_length > 0)
      {
         QString a = response.mid(b.size(), response_length);
         return a;
      }
      else
         return "";
   }

   void QueryError()
   {
      /*
      if (!connected)
         return;

      QString response = SendCommand(controller_index, "TE");

      NewMessage(response);
      */
   }

};