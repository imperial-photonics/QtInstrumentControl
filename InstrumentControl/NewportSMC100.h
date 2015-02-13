#pragma once

#include "GenericNewportController.h"
#include <QThread>
#include <iostream>
/*
Controls Newport SMC100 1 axis stage
http://assets.newport.com/webDocuments-EN/images/SMC100CC_And_SMC100PP_User_Manual.pdf

For reset

1rs  - reset
1ts  - read error
1zx2 - read stage config
1zx3 - enable stage check
1pw1 - enter config state
1sr200 - set limit to 200 (max travel distance)

1pw0 - leave config state
1or  - execute home search



1va16 - set velocity
1pa   - set absolute position
1pa?  - get absolute position

*/
class NewportSMC100 : public GenericNewportController
{
   Q_OBJECT
public:

   NewportSMC100(const QString& stage_type, QObject* parent = 0) :
      GenericNewportController("SMC_PP", stage_type, parent)
   {
      baud = QSerialPort::Baud57600;
      units = "mm";

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

      QByteArray response;
      serial_port->readAll();
      WriteWithTerminator(b);
      response = ReadUntilTerminator(1000);
      
      // Remove question mark from end, isn't returned on SMC100
      if ( b.endsWith("?"))
         b.chop(1);

      //std::cout << "Command: " << b.constData() << "\n";
      //std::cout << "Response: " << response.constData() << "\n";

      int response_length = response.size() - b.size();

      if (response_length > 0)
         return response.mid(b.size(), response_length);
      else
         return "";
   }

   void QueryError()
   {
      if (!connected)
         return;

      QMutexLocker lk(&connection_mutex);

      QString response = SendCommand(controller_index, "TE");

      //!!TODO
      //if (!response.startsWith(c.append(stage_type)))
      //   NewMessage("Unexpected response getting error");

      QString error = response.mid(3, 1);

      if (!error.isEmpty() && error != "@") // No error
      {
         QString error_message = SendCommand(controller_index, QByteArray("TB").append(error));
         QString message = error_message.right(response.size() - 5);

         NewMessage(message);
      }
   }


};