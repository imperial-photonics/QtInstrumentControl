#pragma once

#include <QThread>

class ThreadedObject : public QObject
{
   Q_OBJECT
public:
   ThreadedObject(QObject* parent = 0)
   {
      thread = new QThread(this);

      if (parent != 0)
         connect(parent, &QObject::destroyed, this, &QObject::deleteLater);

      connect(thread, &QThread::started, this, &ThreadedObject::Init);
      connect(this, &QObject::destroyed, thread, &QThread::quit);
      connect(thread, &QThread::finished, thread, &QThread::deleteLater);
      
      this->moveToThread(thread);
      thread->start();
   }

   virtual void Init() {};

protected:
   QThread* thread;
};