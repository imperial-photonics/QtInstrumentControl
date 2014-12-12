#pragma once

#include <QMainWindow>
#include <QTimer>

#include "ui_ArduinoCounterTest.h"

#include "ArduinoCounter.h"
#include "NewportNSC200.h"
#include "NewportSMC100.h"
#include "ThorlabsAPTController.h"
#include "ControlBinder.h"


class ArduinoCounterTest : public QMainWindow, public Ui::MainWindow, public ControlBinder
{
   Q_OBJECT

public:
   ArduinoCounterTest() :
      ControlBinder(nullptr, "CounterTest")
   {
      setupUi(this);

      timer = new QTimer(this);
      

      apt = new ThorlabsAPTController(this);

      //counter = new ArduinoCounter(this);
      //stage = new NewportNSC200("NSR1", this);
      //stage = new NewportSMC100("", this);

      //connect(stage, &SerialDevice::NewMessage, this, &ArduinoCounterTest::DisplayMessage);
      //connect(counter, &SerialDevice::NewMessage, this, &ArduinoCounterTest::DisplayMessage);

      //stage_widget->BindStage(stage);

      timer->setInterval(500);
      timer->start();
      
//      connect(timer, &QTimer::timeout, counter, &ArduinoCounter::GetCount);

   }

   void DisplayCount(int count)
   {
      QString m = QString("Count updated: %1").arg(count);
      debug_text->append(m);
   }

   void DisplayMessage(const QString& msg)
   {
      debug_text->append(msg);
   }

private:
   ArduinoCounter* counter;
   GenericNewportController* stage;
   ThorlabsAPTController* apt;
   QTimer* timer;
};