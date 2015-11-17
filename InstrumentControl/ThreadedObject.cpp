#include "ThreadedObject.h"

ThreadedObject::ThreadedObject(QObject* parent, QThread* ex_thread)
   : thread(ex_thread)
{

   if (parent != nullptr)
      connect(parent, &QObject::destroyed, this, &ThreadedObject::deleteLater);

   private_thread = (thread == nullptr);
   if (thread == nullptr)
      thread = new QThread;

   this->moveToThread(thread);

   // Start thread if it isn't running
   if (!thread->isRunning())
      thread->start();

   if (private_thread)
   {
      connect(this, &QObject::destroyed, thread, &QThread::quit, Qt::DirectConnection);
      connect(this, &QObject::destroyed, thread, &QThread::deleteLater, Qt::DirectConnection);
   }

   //connect(this, &ThreadedObject::Started, this, &ThreadedObject::StartInit, Qt::QueuedConnection);
   //emit Started();
}

void ThreadedObject::StartInit()
{
   QMutexLocker lk(&init_mutex);
   this->init();
   init_cv.wakeAll();
}

void ThreadedObject::StartThread()
{
   thread->setObjectName(objectName());
   
   QMutexLocker lk(&init_mutex);

   // Call Init function
   QMetaObject::invokeMethod(this, "StartInit", Qt::QueuedConnection);
   init_cv.wait(&init_mutex);
}

ThreadedObject::~ThreadedObject()
{
   int a = 0;
}