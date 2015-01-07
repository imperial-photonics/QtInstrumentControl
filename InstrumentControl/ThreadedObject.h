#pragma once

#include <QThread>

class ThreadedObject : public QObject
{
   Q_OBJECT
public:
   ThreadedObject(QObject* parent = 0)
   {
      thread = new QThread(this);
      ConnectSignals();

      if (parent != 0)
         connect(parent, &QObject::destroyed, this, &QObject::deleteLater);
   }

   ThreadedObject(QThread* thread)
      : thread(thread)
   {
      ConnectSignals();
   }

   void StartThread()
   {
      this->moveToThread(thread);
      thread->start();
   }

   QThread* GetThread()
   {
      return thread;
   }

   virtual ~ThreadedObject()
   {
      if (thread->isRunning())
         thread->wait();
   }

   virtual void Init() {};


protected:
   QThread* thread;

private:
   void ConnectSignals()
   {
      connect(thread, &QThread::started, this, &ThreadedObject::Init);
      connect(this, &QObject::destroyed, thread, &QThread::quit);
      connect(thread, &QThread::finished, thread, &QThread::deleteLater);
   }
};

