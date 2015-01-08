#include "ThreadedObject.h"

ThreadedObject::ThreadedObject(QObject* parent, QThread* ex_thread)
   : thread(ex_thread)
{
   if (parent != nullptr)
      connect(parent, &QObject::destroyed, this, &ThreadedObject::deleteLater);

   private_thread = (thread == nullptr);
   if (thread == nullptr)
      thread = new QThread();

}

void ThreadedObject::StartThread()
{
   this->moveToThread(thread);

   // Start thread if it isn't running
   if (!thread->isRunning())
      thread->start();

   if (private_thread)
   {
      connect(this, &QObject::destroyed, thread, &QThread::quit, Qt::DirectConnection);
      connect(this, &QObject::destroyed, thread, &QThread::deleteLater, Qt::DirectConnection);
   }

   // Call Init function
   QMetaObject::invokeMethod(this, "Init", Qt::QueuedConnection);

}

QThread* ThreadedObject::GetThread()
{
   return thread;
}

ThreadedObject::~ThreadedObject()
{
   int a = 0;
}